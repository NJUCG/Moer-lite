#pragma once
#include "Material.h"
#include "NDF/NDF.h"

class ConductorMaterial : public Material {
public:
  ConductorMaterial();

  ConductorMaterial(const Json &json);

  virtual std::shared_ptr<BSDF>
  computeBSDF(const Intersection &intersection) const override;

private:
  std::shared_ptr<Texture<Spectrum>> albedo;
  std::unique_ptr<NDF> ndf;
  Vector3f eta;
  Vector3f k;
  Vector2f roughness;
};