#include "Mirror.h"
#include "./BxDF/Specular.h"

MirrorMaterial::MirrorMaterial(const Json &json) : Material(json) {}

std::shared_ptr<BSDF>
MirrorMaterial::computeBSDF(const Intersection &intersection) const {
  Vector3f normal, tangent, bitangent;
  computeShadingGeometry(intersection, &normal, &tangent, &bitangent);
  return std::make_shared<SpecularReflection>(normal, tangent, bitangent);
}

REGISTER_CLASS(MirrorMaterial, "mirror")