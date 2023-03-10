#include "DirectIntegrator.h"
#include <FunctionLayer/Material/Matte.h>

Spectrum
DirectIntegratorSampleLight::li(Ray &ray, const Scene &scene,
                                std::shared_ptr<Sampler> sampler) const {
  Spectrum spectrum(.0f);
  auto intersectionOpt = scene.rayIntersect(ray);

  if (!intersectionOpt.has_value()) {
    for (auto light : scene.infiniteLights)
      spectrum += light->evaluateEmission(ray);
    return spectrum;
  }
  auto intersection = intersectionOpt.value();
  computeRayDifferentials(&intersection, ray);

  if (auto light = intersection.shape->light; light) {
    spectrum += light->evaluateEmission(intersection, -ray.direction);
  }

  //* 采样所有的环境光
  for (auto light : scene.infiniteLights) {
    auto res = light->sample(intersection, sampler->next2D());
    Ray shadowRay{intersection.position + res.direction * 1e-4f, res.direction,
                  1e-4f, res.distance};
    auto occlude = scene.rayIntersect(shadowRay);
    if (!occlude.has_value()) {
      auto material = intersection.shape->material;
      auto bsdf = material->computeBSDF(intersection);
      Spectrum f = bsdf->f(-ray.direction, shadowRay.direction);
      float pdf = convertPDF(res, intersection);
      spectrum += res.energy * f / pdf;
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
      spectrum += lightSampleResult.energy * f / pdf;
    }
  }
  return spectrum;
}

REGISTER_CLASS(DirectIntegratorSampleLight, "directSampleLight")

Spectrum
DirectIntegratorSampleBSDF ::li(Ray &ray, const Scene &scene,
                                std::shared_ptr<Sampler> sampler) const {
  Spectrum spectrum(.0f);
  auto intersectionOpt = scene.rayIntersect(ray);
  if (!intersectionOpt.has_value()) {
    for (auto light : scene.infiniteLights) {
      spectrum += light->evaluateEmission(ray);
    }
    return spectrum;
  }

  auto intersection = intersectionOpt.value();

  if (auto light = intersection.shape->light; light) {
    spectrum += light->evaluateEmission(intersection, -ray.direction);
  }

  //* 采样BSDF,选择一个入射方向
  auto material = intersection.shape->material;
  auto bsdf = material->computeBSDF(intersection);
  auto bsdfSampleResult = bsdf->sample(-ray.direction, sampler->next2D());

  //* 该入射方向上如果有光源，那么将得到一条有贡献的、长度为1的光路
  Ray shadowRay{intersection.position, bsdfSampleResult.wi};
  auto findLight = scene.rayIntersect(shadowRay);
  if (!findLight.has_value()) {
    //* 计算环境光
    for (auto light : scene.infiniteLights) {
      Spectrum envS = light->evaluateEmission(shadowRay);
      spectrum += bsdfSampleResult.weight * envS;
    }
  } else {
    auto shape = findLight->shape;
    if (auto light = shape->light; light) {
      //* 击中光源
      spectrum +=
          bsdfSampleResult.weight *
          light->evaluateEmission(findLight.value(), -shadowRay.direction);
    }
  }
  return spectrum;
}

REGISTER_CLASS(DirectIntegratorSampleBSDF, "directSampleBSDF")