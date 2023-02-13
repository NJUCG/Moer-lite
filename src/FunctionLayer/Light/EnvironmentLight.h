#pragma once
#include "Light.h"
#include <CoreLayer/Math/Distribution.h>
#include <FunctionLayer/Ray/Ray.h>
#include <FunctionLayer/Texture/ImageTexture.h>
class EnvironmentLight : public InfiniteLight {
public:
  EnvironmentLight() = delete;

  EnvironmentLight(const Json &json);

  Spectrum evaluateEmission(const Ray &ray) const override;

  virtual LightSampleResult sample(const Intersection &shadingPoint,
                                   const Vector2f &sample) const override;

private:
  std::shared_ptr<Texture<Spectrum>> environmentMap;
  Distribution<Vector2i> energyDistribution;
};