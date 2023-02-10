#pragma once
#include "BSDF.h"
#include "Warp.h"
class LambertReflection : public BSDF {
public:
  LambertReflection(const Intersection &intersection, Spectrum _albedo)
      : BSDF(intersection), albedo(_albedo) {}

  virtual Spectrum f(const Vector3f &wo, const Vector3f &wi) const override {
    Vector3f woLocal = toLocal(wo), wiLocal = toLocal(wi);
    if (woLocal[1] <= .0f || wiLocal[1] <= .0f)
      return Spectrum(.0f);
    return albedo * invPI * wiLocal[1];
  }

  virtual BSDFSampleResult sample(const Vector3f &wo,
                                  const Vector2f &sample) const override {
    Spectrum weight = albedo;
    Vector3f wi = squareToUniformHemisphere(sample);
    float pdf = squareToUniformHemispherePdf(wi);
    return {weight, toWorld(wi), pdf};
  }

private:
  Spectrum albedo;
};