#pragma once
#include "./BxDF/BSDF.h"
#include <CoreLayer/Math/Math.h>
#include <FunctionLayer/Shape/Intersection.h>
#include <ResourceLayer/Factory.h>
#include <ResourceLayer/JsonUtil.h>
class Material {
public:
  Material() {
    // donothing
  }

  Material(const Json &json) {
    // donothing
  }

  virtual std::shared_ptr<BSDF>
  computeBSDF(const Intersection &intersection) const = 0;
};