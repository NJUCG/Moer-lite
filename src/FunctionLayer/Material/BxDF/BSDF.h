#pragma once
#include <CoreLayer/ColorSpace/Spectrum.h>

struct BSDFSampleResult {
  Spectrum weight;
  Vector3f wi;
  float pdf;
};

class BSDF {
public:
  virtual Spectrum f(const Vector3f &wo, const Vector3f &wi) const = 0;
  virtual BSDFSampleResult sample(const Vector3f &wo,
                                  const Vector2f &sample) const = 0;
};