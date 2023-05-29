#pragma once

#include "scene/component.h"

struct Material : public Component {
  bool doubleSided{false};
};
