#pragma once
#include "Light.h"
#include <FunctionLayer/Shape/Shape.h>
class AreaLight : public Light {
public:
  AreaLight(const Json &json);

  virtual Spectrum evaluateEmission(const Intersection &intersection,
                                    const Vector3f &wo) const override;
  virtual LightSampleResult sample(const Intersection &shadingPoint,
                                   const Vector2f &sample) const override;

public:
  std::shared_ptr<Shape> shape;

private:
  Spectrum energy;
};