#pragma once
#include "Integrator.h"

class PathIntegrator : public Integrator {
public:
  PathIntegrator() = default;

  PathIntegrator(const Json &json);

  virtual Spectrum li(Ray &ray, const Scene &scene,
                      std::shared_ptr<Sampler> sampler) const override;

private:
  uint32_t maxDepth;
};