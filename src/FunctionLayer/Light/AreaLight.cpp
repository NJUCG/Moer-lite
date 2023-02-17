#include "AreaLight.h"
#include <ResourceLayer/Factory.h>
AreaLight::AreaLight(const Json &json) : Light(json) {
  type = LightType::AreaLight;
  shape = Factory::construct_class<Shape>(json["shape"]);
  energy = fetchRequired<Spectrum>(json, "energy");
}

Spectrum AreaLight::evaluateEmission(const Intersection &intersection,
                                     const Vector3f &wo) const {
  return energy;
}

LightSampleResult AreaLight::sample(const Intersection &shadingPoint,
                                    const Vector2f &sample) const {
  Intersection sampleResult;
  float pdf;
  shape->uniformSampleOnSurface(sample, &sampleResult, &pdf);
  Vector3f shadingPoint2sample = sampleResult.position - shadingPoint.position;

  return {energy,
          normalize(shadingPoint2sample),
          shadingPoint2sample.length() - EPSILON,
          sampleResult.normal,
          pdf,
          false,
          type};
}

REGISTER_CLASS(AreaLight, "areaLight")