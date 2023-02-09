#pragma once
#include "BSDF.h"
#include "Warp.h"
class LambertReflection : public BSDF {
public:
  LambertReflection(Spectrum _albedo) : albedo(_albedo) {}

  virtual Spectrum f(const Vector3f &wo, const Vector3f &wi) const override {
    if (wo[1] <= .0f || wi[1] <= .0f)
      return Spectrum(.0f);
    return albedo * invPI * wi[1];
  }

  virtual BSDFSampleResult sample(const Vector3f &wo,
                                  const Vector2f &sample) const override {
    Spectrum weight = albedo;
    Vector3f wi = squareToUniformHemisphere(sample);
    float pdf = squareToUniformHemispherePdf(wi);
    return {weight, wi, pdf};
  }

private:
  Spectrum albedo;
};