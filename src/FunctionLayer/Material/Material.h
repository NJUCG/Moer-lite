#pragma once
#include "./BxDF/BSDF.h"
#include <CoreLayer/Math/Math.h>
#include <FunctionLayer/Shape/Intersection.h>
#include <FunctionLayer/Texture/NormalTexture.h>
#include <ResourceLayer/Factory.h>
#include <ResourceLayer/JsonUtil.h>
class Material {
public:
  Material() {
    // donothing
  }

  Material(const Json &json) {
    if (json.contains("normalmap"))
      normalMap = std::make_shared<NormalTexture>(json["normalmap"]);
  }

  virtual std::shared_ptr<BSDF>
  computeBSDF(const Intersection &intersection) const = 0;

  void computeShadingGeometry(const Intersection &intersection,
                              Vector3f *normal, Vector3f *tangent,
                              Vector3f *bitangent) const;

protected:
  std::shared_ptr<NormalTexture> normalMap;
};