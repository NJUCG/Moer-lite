#pragma once
#include "Light.h"

class SpotLight : public Light {
public:
  SpotLight() = delete;

  SpotLight(const Json &json);

  virtual Spectrum evaluateEmission(const Intersection &intersection,
                                    const Vector3f &wo) const;

  virtual LightSampleResult sample(const Intersection &shadingPoint,
                                   const Vector2f &sample) const;

private:
  Point3f position;
  Spectrum energy;
};