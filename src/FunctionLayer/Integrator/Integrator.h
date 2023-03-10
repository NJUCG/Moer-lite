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

  virtual Spectrum li(Ray &ray, const Scene &scene,
                      std::shared_ptr<Sampler> sampler) const = 0;
};

//* 将result中各种测度下的pdf都转化为立体角测度下的pdf
inline float convertPDF(const LightSampleResult &result,
                        const Intersection &intersection) {
  float pdf = result.pdf;
  float disance = result.distance;
  switch (result.type) {
  case LightType::SpotLight:
    pdf *= disance * disance;
    break;
  case LightType::AreaLight:
    pdf *= disance * disance;
    pdf /= std::abs(dot(result.normal, result.direction));
    break;
  //* 环境光的pdf转换在采样时已经完成
  case LightType::EnvironmentLight:
    break;
  }
  return pdf;
}