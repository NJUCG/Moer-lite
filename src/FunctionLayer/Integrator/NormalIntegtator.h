#pragma once
#include "Integrator.h"

//* 法线可视化
class NormalIntegrator : public Integrator {
public:
  NormalIntegrator() = default;

  NormalIntegrator(const Json &json) : Integrator(json) {}

  virtual ~NormalIntegrator() = default;

  virtual Spectrum li(const Ray &ray, const Scene &scene) const override;
};
