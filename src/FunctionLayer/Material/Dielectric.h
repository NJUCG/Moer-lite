#pragma once
#include "Material.h"
#include "NDF/NDF.h"

class DielectricMaterial : public Material {
public:
  DielectricMaterial();

  DielectricMaterial(const Json &json);

  virtual std::shared_ptr<BSDF>
  computeBSDF(const Intersection &intersection) const override;

private:
  std::shared_ptr<Texture<Spectrum>> albedo;
  std::unique_ptr<NDF> ndf;
  float eta;
  Vector2f roughness;
};