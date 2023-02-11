#include "DirectIntegrator.h"
#include <FunctionLayer/Material/Matte.h>

std::shared_ptr<Material> material = std::make_shared<MatteMaterial>();

Spectrum DirectIntegrator::li(const Ray &ray, const Scene &scene,
                              std::shared_ptr<Sampler> sampler) const {
  auto intersectionOpt = scene.rayIntersect(ray);
  if (!intersectionOpt.has_value())
    return Spectrum(.0f); // TODO 换为环境光
  auto intersection = intersectionOpt.value();
  float pdfLight = .0f;
  auto light = scene.sampleLight(sampler->next1D(), &pdfLight);
  auto lightSampleResult = light->sample(intersection, sampler->next2D());

  Ray shadowRay{intersection.position, lightSampleResult.position};
  auto occlude = scene.rayIntersect(shadowRay);
  if (!occlude.has_value()) {
    auto bsdf = material->computeBSDF(intersection);
    Spectrum f = bsdf->f(-ray.direction, shadowRay.direction);
    lightSampleResult.pdf *= pdfLight;
    float pdf = convertPDF(lightSampleResult, intersection);
    return lightSampleResult.emission * f / pdf;
  }
  return Spectrum(.0f);
}

REGISTER_CLASS(DirectIntegrator, "direct")