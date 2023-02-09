#include "Matte.h"
#include "./BxDF/Lambert.h"
std::shared_ptr<BSDF>
MatteMaterial::computeBSDF(const Intersection &intersection) const {
  return std::make_shared<LambertReflection>(Spectrum(.6f, .3f, .1f));
}