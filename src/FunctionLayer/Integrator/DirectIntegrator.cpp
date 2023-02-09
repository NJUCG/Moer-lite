#include "DirectIntegrator.h"

Spectrum DirectIntegrator::li(const Ray &ray, const Scene &scene) const {
  auto intersectionOpt = scene.rayIntersect(ray);
  if (!intersectionOpt.has_value())
    return Spectrum(.0f); // TODO 换为环境光

  auto intersection = intersectionOpt.value();
}