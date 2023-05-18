#pragma once

#include "scene/sub_mesh.h"
#include <vector>

struct Mesh : public Component {
  std::vector<SubMesh *> subMeshes;
};
