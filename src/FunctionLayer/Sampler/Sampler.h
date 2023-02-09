#pragma once
#include <CoreLayer/Math/Math.h>
class Sampler {
public:
  virtual float next1D() = 0;

  virtual Vector2f next2D() = 0;
};