#pragma once
#include <CoreLayer/Math/Math.h>
#include <ResourceLayer/JsonUtil.h>
class Sampler {
public:
  Sampler(const Json &json) {
    xSamples = fetchRequired<int>(json, "xSamples");
    ySamples = fetchRequired<int>(json, "ySamples");
  }

  virtual float next1D() = 0;

  virtual Vector2f next2D() = 0;

public:
  int xSamples, ySamples;
};