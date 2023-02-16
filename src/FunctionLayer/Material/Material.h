#pragma once
#include "./BxDF/BSDF.h"
#include <CoreLayer/Math/Math.h>
#include <FunctionLayer/Shape/Intersection.h>
#include <FunctionLayer/Texture/ImageTexture.h>
#include <ResourceLayer/Factory.h>
#include <ResourceLayer/JsonUtil.h>
class Material {
public:
  Material() {
    // donothing
  }

  Material(const Json &json) {
    if (json.contains("normalmap"))
      normalMap = Factory::construct_class<ImageTexture>(json["normalmap"]);
  }

  virtual std::shared_ptr<BSDF>
  computeBSDF(const Intersection &intersection) const = 0;

protected:
  // TODO
  std::shared_ptr<ImageTexture> normalMap;
};