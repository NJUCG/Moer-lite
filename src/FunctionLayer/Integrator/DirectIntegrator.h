#pragma once
#include "Integrator.h"

//* 只计算场景中的直接光照，从光线与几何体的交点采样光源并连接
class DirectIntegrator : public Integrator {
public:
  DirectIntegrator() = default;

  virtual ~DirectIntegrator() = default;

  DirectIntegrator(const Json &json) {}

  virtual Spectrum li(const Ray &ray, const Scene &scene,
                      std::shared_ptr<Sampler> sampler) const override;
};