#include "Matte.h"
#include "./BxDF/Lambert.h"
MatteMaterial::MatteMaterial() {
  // default
  albedo = Spectrum(.5f);
}

MatteMaterial::MatteMaterial(const Json &json) : Material(json) {
  albedo = fetchRequired<Spectrum>(json, "albedo");
}

std::shared_ptr<BSDF>
MatteMaterial::computeBSDF(const Intersection &intersection) const {
  return std::make_shared<LambertReflection>(intersection, albedo);
}

REGISTER_CLASS(MatteMaterial, "matte")