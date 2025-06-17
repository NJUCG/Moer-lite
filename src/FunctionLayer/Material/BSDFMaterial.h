#pragma once
#include "FunctionLayer/Material/BxDF/Disney.h"
#include "Material.h"
#include "ResourceLayer/JsonUtil.h"

class BSDFMaterial : public Material {
public:
  BSDFMaterial(const Json &json);

  virtual std::shared_ptr<BSDF>
  computeBSDF(const Intersection &intersection) const override;

private:
  Json config;
};
