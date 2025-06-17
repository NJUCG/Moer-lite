#include "BSDFMaterial.h"
#include "./BxDF/Disney.h"

#include <FunctionLayer/Texture/ConstantTexture.h>

BSDFMaterial::BSDFMaterial(const Json &json) : Material(json), config(json) {}

std::shared_ptr<BSDF>
BSDFMaterial::computeBSDF(const Intersection &intersection) const {
  Vector3f normal, tangent, bitangent;
  computeShadingGeometry(intersection, &normal, &tangent, &bitangent);
  return std::make_shared<DisneyBSDF>(config, normal, tangent, bitangent);
}

REGISTER_CLASS(BSDFMaterial, "disney")
