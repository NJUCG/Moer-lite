#include "Matte.h"
#include "./BxDF/Lambert.h"
std::shared_ptr<BSDF>
MatteMaterial::computeBSDF(const Intersection &intersection) const {
  return std::make_shared<LambertReflection>(intersection, Spectrum(.5f));
}