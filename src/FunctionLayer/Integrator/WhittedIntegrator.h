#pragma once

#include "Integrator.h"

class WhittedIntegrator : public Integrator {
public:
  WhittedIntegrator() = default;

  WhittedIntegrator(const Json &json) : Integrator(json) {}

  virtual ~WhittedIntegrator() = default;

  virtual Spectrum li(Ray &ray, const Scene &scene,
                      std::shared_ptr<Sampler> sampler) const override;
};