#pragma once
#include <CoreLayer/ColorSpace/Spectrum.h>
#include <FunctionLayer/Ray/Ray.h>
#include <FunctionLayer/Sampler/Sampler.h>
#include <FunctionLayer/Scene/Scene.h>
#include <ResourceLayer/Factory.h>
#include <ResourceLayer/JsonUtil.h>
class Integrator {
public:
  Integrator() = default;

  virtual ~Integrator() = default;

  Integrator(const Json &json) {}

  virtual Spectrum li(const Ray &ray, const Scene &scene,
                      std::shared_ptr<Sampler> sampler) const = 0;
};