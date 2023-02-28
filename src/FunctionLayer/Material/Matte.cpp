#include "Matte.h"
#include "./BxDF/Lambert.h"
#include "./BxDF/Specular.h"
#include <FunctionLayer/Texture/ConstantTexture.h>
MatteMaterial::MatteMaterial() {
  // default
  albedo = std::make_shared<ConstantTexture<Spectrum>>(Spectrum(.5f));
}

MatteMaterial::MatteMaterial(const Json &json) : Material(json) {
  if (json["albedo"].is_object()) {
    //* albedo是一个json对象（表示纹理）
    albedo = Factory::construct_class<Texture<Spectrum>>(json["albedo"]);
  } else if (json["albedo"].is_array()) {
    //* albedo是一个json数组（表示常量albedo）
    auto s = fetchRequired<Spectrum>(json, "albedo");
    albedo = std::make_shared<ConstantTexture<Spectrum>>(s);
  } else {
    std::cerr << "Error in albedo format!\n";
    exit(1);
  }
}

std::shared_ptr<BSDF>
MatteMaterial::computeBSDF(const Intersection &intersection) const {
  Vector3f normal, tangent, bitangent;
  computeShadingGeometry(intersection, &normal, &tangent, &bitangent);

  Spectrum s = albedo->evaluate(intersection);
  return std::make_shared<LambertReflection>(normal, tangent, bitangent, s);
}

REGISTER_CLASS(MatteMaterial, "matte")