#include "scene/sub_mesh.h"

bool SubMesh::getAttribute(const std::string &name,
                           VertexAttribute   &attribute) const {
  if (auto it = vertexAttributes.find(name); it != vertexAttributes.end()) {
    attribute = it->second;
    return true;
  }
  return false;
}
