#pragma once
#include "./BxDF/BSDF.h"
#include <CoreLayer/Math/Math.h>
#include <FunctionLayer/Shape/Intersection.h>
#include <ResourceLayer/JsonUtil.h>

class Material {
public:
  virtual std::shared_ptr<BSDF>
  computeBSDF(const Intersection &intersection) const = 0;
};