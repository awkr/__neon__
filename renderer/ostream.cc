#include "renderer/ostream.h"

std::ostream &operator<<(std::ostream &os, const VkExtent2D &extent) {
  os << "(" << extent.width << ", " << extent.height << ")";
  return os;
}

std::ostream &operator<<(std::ostream &os, const VkFormat &format) {
  switch (format) {
  case VK_FORMAT_R4G4_UNORM_PACK8:
    os << "VK_FORMAT_R4G4_UNORM_PACK8";
    break;
  case VK_FORMAT_R4G4B4A4_UNORM_PACK16:
    os << "VK_FORMAT_R4G4B4A4_UNORM_PACK16";
    break;
  case VK_FORMAT_B4G4R4A4_UNORM_PACK16:
    os << "VK_FORMAT_B4G4R4A4_UNORM_PACK16";
    break;
  case VK_FORMAT_R5G6B5_UNORM_PACK16:
    os << "VK_FORMAT_R5G6B5_UNORM_PACK16";
    break;
  case VK_FORMAT_B5G6R5_UNORM_PACK16:
    os << "VK_FORMAT_B5G6R5_UNORM_PACK16";
    break;
  case VK_FORMAT_R5G5B5A1_UNORM_PACK16:
    os << "VK_FORMAT_R5G5B5A1_UNORM_PACK16";
    break;
  case VK_FORMAT_B5G5R5A1_UNORM_PACK16:
    os << "VK_FORMAT_B5G5R5A1_UNORM_PACK16";
    break;
  case VK_FORMAT_A1R5G5B5_UNORM_PACK16:
    os << "VK_FORMAT_A1R5G5B5_UNORM_PACK16";
    break;
  case VK_FORMAT_R8_UNORM:
    os << "VK_FORMAT_R8_UNORM";
    break;
  case VK_FORMAT_R8_SNORM:
    os << "VK_FORMAT_R8_SNORM";
    break;
  case VK_FORMAT_R8_USCALED:
    os << "VK_FORMAT_R8_USCALED";
    break;
  case VK_FORMAT_R8_SSCALED:
    os << "VK_FORMAT_R8_SSCALED";
    break;
  case VK_FORMAT_R8_UINT:
    os << "VK_FORMAT_R8_UINT";
    break;
  case VK_FORMAT_R8_SINT:
    os << "VK_FORMAT_R8_SINT";
    break;
  case VK_FORMAT_R8_SRGB:
    os << "VK_FORMAT_R8_SRGB";
    break;
  case VK_FORMAT_R8G8_UNORM:
    os << "VK_FORMAT_R8G8_UNORM";
    break;
  case VK_FORMAT_R8G8_SNORM:
    os << "VK_FORMAT_R8G8_SNORM";
    break;
  case VK_FORMAT_R8G8_USCALED:
    os << "VK_FORMAT_R8G8_USCALED";
    break;
  case VK_FORMAT_R8G8_SSCALED:
    os << "VK_FORMAT_R8G8_SSCALED";
    break;
  case VK_FORMAT_R8G8_UINT:
    os << "VK_FORMAT_R8G8_UINT";
    break;
  case VK_FORMAT_R8G8_SINT:
    os << "VK_FORMAT_R8G8_SINT";
    break;
  case VK_FORMAT_R8G8_SRGB:
    os << "VK_FORMAT_R8G8_SRGB";
    break;
  case VK_FORMAT_R8G8B8_UNORM:
    os << "VK_FORMAT_R8G8B8_UNORM";
    break;
  case VK_FORMAT_R8G8B8_SNORM:
    os << "VK_FORMAT_R8G8B8_SNORM";
  case VK_FORMAT_R8G8B8_USCALED:
    os << "VK_FORMAT_R8G8B8_USCALED";
    break;
  case VK_FORMAT_R8G8B8_SSCALED:
    os << "VK_FORMAT_R8G8B8_SSCALED";
    break;
  case VK_FORMAT_R8G8B8_UINT:
    os << "VK_FORMAT_R8G8B8_UINT";
    break;
  case VK_FORMAT_R8G8B8_SINT:
    os << "VK_FORMAT_R8G8B8_SINT";
    break;
  case VK_FORMAT_R8G8B8_SRGB:
    os << "VK_FORMAT_R8G8B8_SRGB";
    break;
  case VK_FORMAT_B8G8R8_UNORM:
    os << "VK_FORMAT_B8G8R8_UNORM";
    break;
  case VK_FORMAT_B8G8R8_SNORM:
    os << "VK_FORMAT_B8G8R8_SNORM";
    break;
  case VK_FORMAT_B8G8R8_USCALED:
    os << "VK_FORMAT_B8G8R8_USCALED";
    break;
  case VK_FORMAT_B8G8R8_SSCALED:
    os << "VK_FORMAT_B8G8R8_SSCALED";
    break;
  case VK_FORMAT_B8G8R8_UINT:
    os << "VK_FORMAT_B8G8R8_UINT";
    break;
  case VK_FORMAT_B8G8R8_SINT:
    os << "VK_FORMAT_B8G8R8_SINT";
    break;
  case VK_FORMAT_B8G8R8_SRGB:
    os << "VK_FORMAT_B8G8R8_SRGB";
    break;
  case VK_FORMAT_R8G8B8A8_UNORM:
    os << "VK_FORMAT_R8G8B8A8_UNORM";
    break;
  case VK_FORMAT_R8G8B8A8_SNORM:
    os << "VK_FORMAT_R8G8B8A8_SNORM";
    break;
  case VK_FORMAT_R8G8B8A8_USCALED:
    os << "VK_FORMAT_R8G8B8A8_USCALED";
    break;
  case VK_FORMAT_R8G8B8A8_SSCALED:
    os << "VK_FORMAT_R8G8B8A8_SSCALED";
    break;
  case VK_FORMAT_R8G8B8A8_UINT:
    os << "VK_FORMAT_R8G8B8A8_UINT";
    break;
  case VK_FORMAT_R8G8B8A8_SINT:
    os << "VK_FORMAT_R8G8B8A8_SINT";
    break;
  case VK_FORMAT_R8G8B8A8_SRGB:
    os << "VK_FORMAT_R8G8B8A8_SRGB";
    break;
  case VK_FORMAT_B8G8R8A8_UNORM:
    os << "VK_FORMAT_B8G8R8A8_UNORM";
    break;
  case VK_FORMAT_B8G8R8A8_SNORM:
    os << "VK_FORMAT_B8G8R8A8_SNORM";
    break;
  case VK_FORMAT_B8G8R8A8_USCALED:
    os << "VK_FORMAT_B8G8R8A8_USCALED";
    break;
  case VK_FORMAT_B8G8R8A8_SSCALED:
    os << "VK_FORMAT_B8G8R8A8_SSCALED";
    break;
  case VK_FORMAT_B8G8R8A8_UINT:
    os << "VK_FORMAT_B8G8R8A8_UINT";
    break;
  case VK_FORMAT_B8G8R8A8_SINT:
    os << "VK_FORMAT_B8G8R8A8_SINT";
    break;
  case VK_FORMAT_B8G8R8A8_SRGB:
    os << "VK_FORMAT_B8G8R8A8_SRGB";
    break;
  case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
    os << "VK_FORMAT_A8B8G8R8_UNORM_PACK32";
    break;
  case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
    os << "VK_FORMAT_A8B8G8R8_SNORM_PACK32";
    break;
  case VK_FORMAT_A8B8G8R8_USCALED_PACK32:
    os << "VK_FORMAT_A8B8G8R8_USCALED_PACK32";
    break;
  case VK_FORMAT_A8B8G8R8_SSCALED_PACK32:
    os << "VK_FORMAT_A8B8G8R8_SSCALED_PACK32";
    break;
  case VK_FORMAT_A8B8G8R8_UINT_PACK32:
    os << "VK_FORMAT_A8B8G8R8_UINT_PACK32";
    break;
  case VK_FORMAT_A8B8G8R8_SINT_PACK32:
    os << "VK_FORMAT_A8B8G8R8_SINT_PACK32";
    break;
  case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
    os << "VK_FORMAT_A8B8G8R8_SRGB_PACK32";
    break;
  case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
    os << "VK_FORMAT_A2R10G10B10_UNORM_PACK32";
    break;
  case VK_FORMAT_A2R10G10B10_SNORM_PACK32:
    os << "VK_FORMAT_A2R10G10B10_SNORM_PACK32";
    break;
  case VK_FORMAT_A2R10G10B10_USCALED_PACK32:
    os << "VK_FORMAT_A2R10G10B10_USCALED_PACK32";
    break;
  case VK_FORMAT_A2R10G10B10_SSCALED_PACK32:
    os << "VK_FORMAT_A2R10G10B10_SSCALED_PACK32";
    break;
  case VK_FORMAT_A2R10G10B10_UINT_PACK32:
    os << "VK_FORMAT_A2R10G10B10_UINT_PACK32";
    break;
  case VK_FORMAT_A2R10G10B10_SINT_PACK32:
    os << "VK_FORMAT_A2R10G10B10_SINT_PACK32";
    break;
  case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
    os << "VK_FORMAT_A2B10G10R10_UNORM_PACK32";
    break;
  case VK_FORMAT_A2B10G10R10_SNORM_PACK32:
    os << "VK_FORMAT_A2B10G10R10_SNORM_PACK32";
    break;
  case VK_FORMAT_A2B10G10R10_USCALED_PACK32:
    os << "VK_FORMAT_A2B10G10R10_USCALED_PACK32";
    break;
  case VK_FORMAT_A2B10G10R10_SSCALED_PACK32:
    os << "VK_FORMAT_A2B10G10R10_SSCALED_PACK32";
    break;
  case VK_FORMAT_A2B10G10R10_UINT_PACK32:
    os << "VK_FORMAT_A2B10G10R10_UINT_PACK32";
    break;
  case VK_FORMAT_A2B10G10R10_SINT_PACK32:
    os << "VK_FORMAT_A2B10G10R10_SINT_PACK32";
    break;
  case VK_FORMAT_R16_UNORM:
    os << "VK_FORMAT_R16_UNORM";
    break;
  case VK_FORMAT_R16_SNORM:
    os << "VK_FORMAT_R16_SNORM";
    break;
  case VK_FORMAT_R16_USCALED:
    os << "VK_FORMAT_R16_USCALED";
    break;
  case VK_FORMAT_R16_SSCALED:
    os << "VK_FORMAT_R16_SSCALED";
    break;
  case VK_FORMAT_R16_UINT:
    os << "VK_FORMAT_R16_UINT";
    break;
  case VK_FORMAT_R16_SINT:
    os << "VK_FORMAT_R16_SINT";
    break;
  case VK_FORMAT_R16_SFLOAT:
    os << "VK_FORMAT_R16_SFLOAT";
    break;
  case VK_FORMAT_R16G16_UNORM:
    os << "VK_FORMAT_R16G16_UNORM";
    break;
  case VK_FORMAT_R16G16_SNORM:
    os << "VK_FORMAT_R16G16_SNORM";
    break;
  case VK_FORMAT_R16G16_USCALED:
    os << "VK_FORMAT_R16G16_USCALED";
    break;
  case VK_FORMAT_R16G16_SSCALED:
    os << "VK_FORMAT_R16G16_SSCALED";
    break;
  case VK_FORMAT_R16G16_UINT:
    os << "VK_FORMAT_R16G16_UINT";
    break;
  case VK_FORMAT_R16G16_SINT:
    os << "VK_FORMAT_R16G16_SINT";
    break;
  case VK_FORMAT_R16G16_SFLOAT:
    os << "VK_FORMAT_R16G16_SFLOAT";
    break;
  case VK_FORMAT_R16G16B16_UNORM:
    os << "VK_FORMAT_R16G16B16_UNORM";
    break;
  case VK_FORMAT_R16G16B16_SNORM:
    os << "VK_FORMAT_R16G16B16_SNORM";
    break;
  case VK_FORMAT_R16G16B16_USCALED:
    os << "VK_FORMAT_R16G16B16_USCALED";
    break;
  case VK_FORMAT_R16G16B16_SSCALED:
    os << "VK_FORMAT_R16G16B16_SSCALED";
    break;
  case VK_FORMAT_R16G16B16_UINT:
    os << "VK_FORMAT_R16G16B16_UINT";
    break;
  case VK_FORMAT_R16G16B16_SINT:
    os << "VK_FORMAT_R16G16B16_SINT";
    break;
  case VK_FORMAT_R16G16B16_SFLOAT:
    os << "VK_FORMAT_R16G16B16_SFLOAT";
    break;
  case VK_FORMAT_R16G16B16A16_UNORM:
    os << "VK_FORMAT_R16G16B16A16_UNORM";
    break;
  case VK_FORMAT_R16G16B16A16_SNORM:
    os << "VK_FORMAT_R16G16B16A16_SNORM";
    break;
  case VK_FORMAT_R16G16B16A16_USCALED:
    os << "VK_FORMAT_R16G16B16A16_USCALED";
    break;
  case VK_FORMAT_R16G16B16A16_SSCALED:
    os << "VK_FORMAT_R16G16B16A16_SSCALED";
    break;
  case VK_FORMAT_R16G16B16A16_UINT:
    os << "VK_FORMAT_R16G16B16A16_UINT";
    break;
  case VK_FORMAT_R16G16B16A16_SINT:
    os << "VK_FORMAT_R16G16B16A16_SINT";
    break;
  case VK_FORMAT_R16G16B16A16_SFLOAT:
    os << "VK_FORMAT_R16G16B16A16_SFLOAT";
    break;
  case VK_FORMAT_R32_UINT:
    os << "VK_FORMAT_R32_UINT";
    break;
  case VK_FORMAT_R32_SINT:
    os << "VK_FORMAT_R32_SINT";
    break;
  case VK_FORMAT_R32_SFLOAT:
    os << "VK_FORMAT_R32_SFLOAT";
    break;
  case VK_FORMAT_R32G32_UINT:
    os << "VK_FORMAT_R32G32_UINT";
    break;
  case VK_FORMAT_R32G32_SINT:
    os << "VK_FORMAT_R32G32_SINT";
    break;
  case VK_FORMAT_R32G32_SFLOAT:
    os << "VK_FORMAT_R32G32_SFLOAT";
    break;
  case VK_FORMAT_R32G32B32_UINT:
    os << "VK_FORMAT_R32G32B32_UINT";
    break;
  case VK_FORMAT_R32G32B32_SINT:
    os << "VK_FORMAT_R32G32B32_SINT";
    break;
  case VK_FORMAT_R32G32B32_SFLOAT:
    os << "VK_FORMAT_R32G32B32_SFLOAT";
    break;
  case VK_FORMAT_R32G32B32A32_UINT:
    os << "VK_FORMAT_R32G32B32A32_UINT";
    break;
  case VK_FORMAT_R32G32B32A32_SINT:
    os << "VK_FORMAT_R32G32B32A32_SINT";
    break;
  case VK_FORMAT_R32G32B32A32_SFLOAT:
    os << "VK_FORMAT_R32G32B32A32_SFLOAT";
    break;
  case VK_FORMAT_R64_UINT:
    os << "VK_FORMAT_R64_UINT";
    break;
  case VK_FORMAT_R64_SINT:
    os << "VK_FORMAT_R64_SINT";
    break;
  case VK_FORMAT_R64_SFLOAT:
    os << "VK_FORMAT_R64_SFLOAT";
    break;
  case VK_FORMAT_R64G64_UINT:
    os << "VK_FORMAT_R64G64_UINT";
    break;
  case VK_FORMAT_R64G64_SINT:
    os << "VK_FORMAT_R64G64_SINT";
    break;
  case VK_FORMAT_R64G64_SFLOAT:
    os << "VK_FORMAT_R64G64_SFLOAT";
    break;
  case VK_FORMAT_R64G64B64_UINT:
    os << "VK_FORMAT_R64G64B64_UINT";
    break;
  case VK_FORMAT_R64G64B64_SINT:
    os << "VK_FORMAT_R64G64B64_SINT";
    break;
  case VK_FORMAT_R64G64B64_SFLOAT:
    os << "VK_FORMAT_R64G64B64_SFLOAT";
    break;
  case VK_FORMAT_R64G64B64A64_UINT:
    os << "VK_FORMAT_R64G64B64A64_UINT";
    break;
  case VK_FORMAT_R64G64B64A64_SINT:
    os << "VK_FORMAT_R64G64B64A64_SINT";
    break;
  case VK_FORMAT_R64G64B64A64_SFLOAT:
    os << "VK_FORMAT_R64G64B64A64_SFLOAT";
    break;
  case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
    os << "VK_FORMAT_B10G11R11_UFLOAT_PACK32";
    break;
  case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:
    os << "VK_FORMAT_E5B9G9R9_UFLOAT_PACK32";
    break;
  case VK_FORMAT_D16_UNORM:
    os << "VK_FORMAT_D16_UNORM";
    break;
  case VK_FORMAT_X8_D24_UNORM_PACK32:
    os << "VK_FORMAT_X8_D24_UNORM_PACK32";
    break;
  case VK_FORMAT_D32_SFLOAT:
    os << "VK_FORMAT_D32_SFLOAT";
    break;
  case VK_FORMAT_S8_UINT:
    os << "VK_FORMAT_S8_UINT";
    break;
  case VK_FORMAT_D16_UNORM_S8_UINT:
    os << "VK_FORMAT_D16_UNORM_S8_UINT";
    break;
  case VK_FORMAT_D24_UNORM_S8_UINT:
    os << "VK_FORMAT_D24_UNORM_S8_UINT";
    break;
  case VK_FORMAT_D32_SFLOAT_S8_UINT:
    os << "VK_FORMAT_D32_SFLOAT_S8_UINT";
    break;
  case VK_FORMAT_UNDEFINED:
    os << "VK_FORMAT_UNDEFINED";
    break;
  default:
    os << "VK_FORMAT_INVALID";
    break;
  }
  return os;
}

std::ostream &operator<<(std::ostream &os,
                         const VkSurfaceFormatKHR &surfaceFormat) {
  os << "(" << surfaceFormat.format << ", ";

  switch (surfaceFormat.colorSpace) {
  case VK_COLORSPACE_SRGB_NONLINEAR_KHR:
    os << "VK_COLORSPACE_SRGB_NONLINEAR_KHR";
    break;
  default:
    os << "<Unknown color space>";
  }
  os << ")";
  return os;
}

std::ostream &operator<<(std::ostream &os,
                         VkSurfaceTransformFlagBitsKHR surfaceTransform) {
  switch (surfaceTransform) {
  case VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR:
    os << "SURFACE_TRANSFORM_IDENTITY";
    break;
  case VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR:
    os << "SURFACE_TRANSFORM_ROTATE_90";
    break;
  case VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR:
    os << "SURFACE_TRANSFORM_ROTATE_180";
    break;
  case VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR:
    os << "SURFACE_TRANSFORM_ROTATE_270";
    break;
  case VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR:
    os << "SURFACE_TRANSFORM_HORIZONTAL_MIRROR";
    break;
  case VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR:
    os << "SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90";
    break;
  case VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR:
    os << "SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180";
    break;
  case VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR:
    os << "SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270";
    break;
  case VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR:
    os << "SURFACE_TRANSFORM_INHERIT";
    break;
  case VK_SURFACE_TRANSFORM_FLAG_BITS_MAX_ENUM_KHR:
    os << "SURFACE_TRANSFORM_FLAG_BITS_MAX_ENUM";
    break;
  default:
    os << "<Unknown surface transform>";
    break;
  }
  return os;
}

std::ostream &operator<<(std::ostream &os,
                         VkCompositeAlphaFlagBitsKHR compositeAlpha) {
  switch (compositeAlpha) {
  case VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR:
    os << "VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR";
    break;
  case VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR:
    os << "VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR";
    break;
  case VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR:
    os << "VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR";
    break;
  case VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR:
    os << "VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR";
    break;
  case VK_COMPOSITE_ALPHA_FLAG_BITS_MAX_ENUM_KHR:
    os << "VK_COMPOSITE_ALPHA_FLAG_BITS_MAX_ENUM_KHR";
    break;
  default:
    os << "<Unknown composite alpha>";
    break;
  }
  return os;
}
