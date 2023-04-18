#pragma once
#include "Material.h"

class PhongMaterial : public Material {
public:
  PhongMaterial();

  PhongMaterial(const Json &json);

  virtual std::shared_ptr<BSDF>
  computeBSDF(const Intersection &intersection) const override;

private:
  std::shared_ptr<Texture<Spectrum>> albedo;
  float kd; // 漫反射系数，高光（镜面反射）系数ks = 1 - kd
  float p;  // 高光衰减系数
};
