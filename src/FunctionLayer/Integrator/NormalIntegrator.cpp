#include "NormalIntegtator.h"

Spectrum NormalIntegrator::li(const Ray &ray, const Scene &scene) const {
  auto intersectionOpt = scene.rayIntersect(ray);
  if (intersectionOpt.has_value())
    return Spectrum(intersectionOpt.value().normal + Vector3f(1.f)) * .5f;
  return Spectrum(.0f);
}
REGISTER_CLASS(NormalIntegrator, "normal")