#include "renderer/resource_binding_state.h"

void ResourceBindingState::reset() {
  clearDirty();
  resourceSets.clear();
}

void ResourceBindingState::clearDirty() { dirty = false; }
