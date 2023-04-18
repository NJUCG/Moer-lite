#pragma once
#include "Material.h"

class OrenNayarMaterial : public Material {
public:
  OrenNayarMaterial();

  OrenNayarMaterial(const Json &json);

  virtual std::shared_ptr<BSDF>
  computeBSDF(const Intersection &intersection) const override;

private:
  std::shared_ptr<Texture<Spectrum>> albedo;
  float roughness;
};
