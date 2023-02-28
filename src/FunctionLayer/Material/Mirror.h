#pragma once
#include "Material.h"

class MirrorMaterial : public Material {
public:
  MirrorMaterial();

  MirrorMaterial(const Json &json);

  virtual std::shared_ptr<BSDF>
  computeBSDF(const Intersection &intersection) const override;
};
