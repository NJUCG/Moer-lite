#pragma once
#include "BSDF.h"

class SpecularReflection : public BSDF {
public:
  SpecularReflection(const Vector3f &_normal, const Vector3f &_tangent,
                     const Vector3f &_bitangent)
      : BSDF(_normal, _tangent, _bitangent) {}

  virtual Spectrum f(const Vector3f &wo, const Vector3f &wi) const override {
    return Spectrum(.0f);
  }

  virtual BSDFSampleResult sample(const Vector3f &wo,
                                  const Vector2f &sample) const override {
    Vector3f woLocal = toLocal(wo);
    Vector3f wiLocal{-woLocal[0], woLocal[1], -woLocal[2]};
    return {Spectrum(1.f), toWorld(wiLocal), 1.f, BSDFType::Specular};
  }
};
