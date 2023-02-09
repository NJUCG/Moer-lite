#include "SpotLight.h"
#include <ResourceLayer/Factory.h>

SpotLight::SpotLight(const Json &json) : Light(json) {
  position = fetchRequired<Point3f>(json, "position");
  energy = fetchRequired<Spectrum>(json, "energy");
}

//! 由于点光源不会与光线发生相交，故该函数实际上不会被调用
Spectrum SpotLight::evaluateEmission(const Intersection &intersection,
                                     const Vector3f &wo) const {
  return Spectrum(.0f);
}

LightSampleResult SpotLight::sample(const Intersection &shadingPoint,
                                    const Vector2f &sample) const {
  return LightSampleResult{energy, position, Vector3f(), 1.f, true};
}

REGISTER_CLASS(SpotLight, "spotLight")