#pragma once

struct Resource {
  virtual ~Resource() = default;
  virtual bool prepare() = 0;
};
