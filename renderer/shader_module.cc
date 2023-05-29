#include "renderer/shader_module.h"
#include "core/file_system.h"
#include "core/string_utils.h"
#include "renderer/device.h"
#include <SPIRV/GlslangToSpv.h>
#include <glslang/Public/ResourceLimits.h>
#include <iostream>

bool createShaderSource(ShaderSource      *shaderSource,
                        const std::string &filepath) {
  std::string source{};
  if (!readFile(filepath, source)) { return false; }

  std::hash<std::string> hasher{};
  shaderSource->id = hasher(std::string{source.cbegin(), source.cend()});

  shaderSource->filepath = filepath;
  shaderSource->source   = std::move(source);

  return true;
}

// Pre-compile shader files to include header code
bool precompileShader(const std::string        &source,
                      std::vector<std::string> &finalSource) {
  auto lines = split(source, '\n');

  for (auto &line : lines) {
    trim(line);
    if (line.find("#include \"") == 0) {
      std::string includePath = line.substr(10);
      size_t      lastQuote   = includePath.find('"');
      if (!includePath.empty() && lastQuote != std::string::npos) {
        includePath = includePath.substr(0, lastQuote);
      }

      std::string code{};
      if (!readFile(includePath, code)) { return false; }

      std::vector<std::string> includedFile;
      if (!precompileShader(code, includedFile)) { return false; }

      for (const auto &includedFileLine : includedFile) {
        finalSource.push_back(includedFileLine);
      }
    } else {
      finalSource.push_back(line);
    }
  }

  return true;
}

std::vector<uint8_t> convertToBytes(std::vector<std::string> &lines) {
  std::vector<uint8_t> bytes;

  for (auto &line : lines) {
    line += "\n";
    std::vector<uint8_t> lineBytes(line.begin(), line.end());
    bytes.insert(bytes.end(), lineBytes.begin(), lineBytes.end());
  }

  return bytes;
}

inline EShLanguage findShaderLanguage(VkShaderStageFlagBits stage) {
  switch (stage) {
  case VK_SHADER_STAGE_VERTEX_BIT:
    return EShLangVertex;

  case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
    return EShLangTessControl;

  case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
    return EShLangTessEvaluation;

  case VK_SHADER_STAGE_GEOMETRY_BIT:
    return EShLangGeometry;

  case VK_SHADER_STAGE_FRAGMENT_BIT:
    return EShLangFragment;

  case VK_SHADER_STAGE_COMPUTE_BIT:
    return EShLangCompute;

  case VK_SHADER_STAGE_RAYGEN_BIT_KHR:
    return EShLangRayGen;

  case VK_SHADER_STAGE_ANY_HIT_BIT_KHR:
    return EShLangAnyHit;

  case VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR:
    return EShLangClosestHit;

  case VK_SHADER_STAGE_MISS_BIT_KHR:
    return EShLangMiss;

  case VK_SHADER_STAGE_INTERSECTION_BIT_KHR:
    return EShLangIntersect;

  case VK_SHADER_STAGE_CALLABLE_BIT_KHR:
    return EShLangCallable;

  case VK_SHADER_STAGE_MESH_BIT_EXT:
    return EShLangMesh;

  case VK_SHADER_STAGE_TASK_BIT_EXT:
    return EShLangTask;

  default:
    return EShLangVertex;
  }
}

bool compileToSpirv(VkShaderStageFlagBits       stage,
                    const std::vector<uint8_t> &glsl, const std::string &entry,
                    const ShaderVariant        &variant,
                    std::vector<std::uint32_t> &spirv, std::string &infoLog) {
  glslang::InitializeProcess(); // Initialize glslang library

  auto messages = static_cast<EShMessages>(EShMsgDefault | EShMsgVulkanRules |
                                           EShMsgSpvRules);

  auto lang   = findShaderLanguage(stage);
  auto source = std::string(glsl.begin(), glsl.end());

  const char *filenames[1] = {""};
  const char *shaderSource = reinterpret_cast<const char *>(source.data());

  glslang::TShader shader(lang);
  shader.setStringsWithLengthsAndNames(&shaderSource, nullptr, filenames, 1);
  shader.setEntryPoint(entry.c_str());
  shader.setSourceEntryPoint(entry.c_str());
  shader.setPreamble(variant.getPreamble().c_str());
  shader.addProcesses(variant.getProcesses());
  shader.setEnvTarget(glslang::EshTargetSpv, glslang::EShTargetSpv_1_5);

  if (!shader.parse(GetDefaultResources(), 100, false, messages)) {
    infoLog = std::string(shader.getInfoLog()) + "\n" +
              std::string(shader.getInfoDebugLog());
    trim(infoLog);
    return false;
  }

  glslang::TProgram program;
  program.addShader(&shader);

  if (!program.link(messages)) {
    infoLog = std::string(program.getInfoLog()) + "\n" +
              std::string(program.getInfoDebugLog());
    trim(infoLog);
    return false;
  }

  // Save any info log that was generated
  if (shader.getInfoLog()) {
    std::string message{std::string(shader.getInfoLog()) + "\n" +
                        std::string(shader.getInfoDebugLog())};
    infoLog += trim(message) + "\n";
  }

  if (program.getInfoLog()) {
    std::string message{std::string(program.getInfoLog()) + "\n" +
                        std::string(program.getInfoDebugLog())};
    infoLog += trim(message) + "\n";
  }

  auto intermediate = program.getIntermediate(lang);

  // Translate to SPIRV
  if (!intermediate) {
    infoLog += "Failed to get shared intermediate code.\n";
    return false;
  }

  spv::SpvBuildLogger logger;

  glslang::GlslangToSpv(*intermediate, spirv, &logger);

  infoLog += logger.getAllMessages() + "\n";

  glslang::FinalizeProcess(); // Shutdown glslang library

  return true;
}

void ShaderVariant::addDefinitions(
    const std::vector<std::pair<std::string, std::string>> &definitions) {
  for (const auto &definition : definitions) { addDefinition(definition); }
}

void ShaderVariant::addDefinition(
    const std::pair<std::string, std::string> &definition) {
  const auto &identifier = definition.first;
  const auto &token      = definition.second;

  preamble.append("#define " + identifier + " " + token + "\n");
  processes.emplace_back("D" + identifier + " " + token);

  updateId();
}

void ShaderVariant::updateId() {
  std::hash<std::string> hasher{};
  id = hasher(preamble);
}

std::unique_ptr<ShaderModule> ShaderModule::make(Device               &device,
                                                 VkShaderStageFlagBits stage,
                                                 const ShaderSource   &source,
                                                 const std::string    &entry,
                                                 const ShaderVariant &variant) {
  std::vector<std::string> finalSource{};
  if (!precompileShader(source.source, finalSource)) { return nullptr; }

  std::vector<uint32_t> spirv{};
  std::string           infoLog{};
  if (!compileToSpirv(stage, convertToBytes(finalSource), entry, variant, spirv,
                      infoLog)) {
    std::cout << "[Shader] Make shader module failed: " << infoLog << std::endl;
    return nullptr;
  }

  // Generate a unique id, determined by source and variant
  std::hash<std::string> hasher{};
  size_t                 id = hasher(
                      std::string{reinterpret_cast<const char *>(spirv.data()),
                  reinterpret_cast<const char *>(spirv.data() + spirv.size())});

  auto shaderModule        = std::make_unique<ShaderModule>();
  shaderModule->id         = id;
  shaderModule->stage      = stage;
  shaderModule->spirv      = std::move(spirv);
  shaderModule->entryPoint = entry;
  return std::move(shaderModule);
}
