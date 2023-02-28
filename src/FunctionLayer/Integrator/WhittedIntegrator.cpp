#include "WhittedIntegrator.h"
#include <FunctionLayer/Material/Material.h>

Spectrum WhittedIntegrator::li(const Ray &_ray, const Scene &scene,
                               std::shared_ptr<Sampler> sampler) const {
  Spectrum spectrum(.0f), beta(.0f);
  Ray ray(_ray);

  do {
    auto itsOpt = scene.rayIntersect(ray);

    // escape the scene
    if (!itsOpt.has_value()) {
      for (auto light : scene.infiniteLights) {
        spectrum += light->evaluateEmission(ray);
      }
      break;
    }

    Intersection its = itsOpt.value();
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
      }
    }
  } while (1);

  return spectrum;
}