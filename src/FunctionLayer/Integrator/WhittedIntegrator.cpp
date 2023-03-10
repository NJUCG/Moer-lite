#include "WhittedIntegrator.h"
#include <FunctionLayer/Material/Material.h>

Spectrum WhittedIntegrator::li(Ray &ray, const Scene &scene,
                               std::shared_ptr<Sampler> sampler) const {
  Spectrum spectrum(.0f), beta(1.0f);
  do {
    auto itsOpt = scene.rayIntersect(ray);

    // escape the scene
    if (!itsOpt.has_value()) {
      for (auto light : scene.infiniteLights) {
        spectrum += beta * light->evaluateEmission(ray);
      }
      break;
    }

    Intersection its = itsOpt.value();
    if (auto light = its.shape->light; light) {
      spectrum += beta * light->evaluateEmission(its, -ray.direction);
    }
    computeRayDifferentials(&its, ray);
    auto bsdf = its.shape->material->computeBSDF(its);

    auto bsdfSampleResult = bsdf->sample(-ray.direction, sampler->next2D());

    // If the surface is specular, spwan the ray
    if (bsdfSampleResult.type == BSDFType::Specular) {
      ray = Ray(its.position, bsdfSampleResult.wi);
      beta *= bsdfSampleResult.weight;
      continue;
    }
    // If the surface is not specular, sample the light
    else {
      // First, sample infinite light
      for (auto light : scene.infiniteLights) {
        auto lightSampleResult = light->sample(its, sampler->next2D());
        Ray shadowRay(its.position, lightSampleResult.direction, 1e-4f,
                      FLT_MAX);
        // Successfully connect the light source
        if (auto occlude = scene.rayIntersect(shadowRay);
            !occlude.has_value()) {
          Spectrum f = bsdf->f(-ray.direction, shadowRay.direction);
          float pdf = convertPDF(lightSampleResult, its);
          spectrum += beta * lightSampleResult.energy * f / pdf;
        }
      }

      float pdfLight = .0f;
      // Second, sample the light in scene
      auto light = scene.sampleLight(sampler->next1D(), &pdfLight);
      if (light && pdfLight != .0f) {
        auto lightSampleResult = light->sample(its, sampler->next2D());
        Ray shadowRay(its.position, lightSampleResult.direction, 1e-4f,
                      lightSampleResult.distance);
        // Successfully connect the light source
        if (auto occlude = scene.rayIntersect(shadowRay);
            !occlude.has_value()) {
          Spectrum f = bsdf->f(-ray.direction, shadowRay.direction);
          lightSampleResult.pdf *= pdfLight;
          float pdf = convertPDF(lightSampleResult, its);
          spectrum += beta * lightSampleResult.energy * f / pdf;
        }
      }
      break;
    }
  } while (1);

  return spectrum;
}

REGISTER_CLASS(WhittedIntegrator, "whitted")