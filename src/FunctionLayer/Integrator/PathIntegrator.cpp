#include "PathIntegrator.h"
#include <FunctionLayer/Material/BxDF/Warp.h>
#include <FunctionLayer/Material/Matte.h>

Spectrum PathIntegrator::li(Ray &ray, const Scene &scene,
                            std::shared_ptr<Sampler> sampler) const {
  Spectrum spectrum(.0f);
  Spectrum throughput(1.f);
  auto intersectionOpt = scene.rayIntersect(ray);

  auto depth = 0u;
  bool specularBounce = false;

  while (true) {
    if (!intersectionOpt.has_value()) {
      for (auto light : scene.infiniteLights)
        spectrum += throughput * light->evaluateEmission(ray);
      return spectrum;
    }

    auto intersection = intersectionOpt.value();
    computeRayDifferentials(&intersection, ray);

    if (depth == 0 || specularBounce)
      if (auto light = intersection.shape->light; light) {
        spectrum += light->evaluateEmission(intersection, -ray.direction);
      }

    ++depth;
    if (depth >= maxDepth)
      break;

    //* 采样所有的环境光
    for (auto light : scene.infiniteLights) {
      auto res = light->sample(intersection, sampler->next2D());
      Ray shadowRay{intersection.position + res.direction * 1e-4f,
                    res.direction, 1e-4f, res.distance};
      auto occlude = scene.rayIntersect(shadowRay);
      if (!occlude.has_value()) {
        auto material = intersection.shape->material;
        auto bsdf = material->computeBSDF(intersection);
        Spectrum f = bsdf->f(-ray.direction, shadowRay.direction);
        float pdf = convertPDF(res, intersection);
        spectrum += throughput * res.energy * f / pdf;
      }
    }

    //* 从场景中采样一个光源
    float pdfLight = .0f;
    auto light = scene.sampleLight(sampler->next1D(), &pdfLight);

    if (light && pdfLight != .0f) {
      //* 在光源上采样出一个点
      auto lightSampleResult = light->sample(intersection, sampler->next2D());

      //* 测试shadowray是否被场景遮挡
      Ray shadowRay{intersection.position, lightSampleResult.direction, 1e-4f,
                    lightSampleResult.distance};
      auto occlude = scene.rayIntersect(shadowRay);
      if (!occlude.has_value()) {
        auto material = intersection.shape->material;
        auto bsdf = material->computeBSDF(intersection);
        Spectrum f = bsdf->f(-ray.direction, shadowRay.direction);
        lightSampleResult.pdf *= pdfLight;
        float pdf = convertPDF(lightSampleResult, intersection);
        spectrum += throughput * lightSampleResult.energy * f / pdf;
      }
    }

    if (depth > 2 && sampler->next1D() > 0.95f)
      break;
    throughput /= 0.95f;

    auto bsdf = intersection.shape->material->computeBSDF(intersection);
    auto bsdf_sample_result = bsdf->sample(-ray.direction, sampler->next2D());
    if (bsdf_sample_result.weight.isZero())
      break;

    throughput *= bsdf_sample_result.weight;
    ray = Ray{intersection.position, bsdf_sample_result.wi};

    specularBounce = bsdf_sample_result.type == BSDFType::Specular;
    intersectionOpt = scene.rayIntersect(ray);
  }

  return spectrum;
}

PathIntegrator::PathIntegrator(const Json &json) {
  maxDepth = fetchRequired<uint32_t>(json, "maxDepth");
}

REGISTER_CLASS(PathIntegrator, "path")