#pragma once
#include "Material.h"

class MatteMaterial : public Material {
public:
  virtual std::shared_ptr<BSDF>
  computeBSDF(const Intersection &intersection) const override;

private:
  Spectrum diffuse;
};