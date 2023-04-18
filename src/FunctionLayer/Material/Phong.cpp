#include "Phong.h"
#include "./BxDF/Phong.h"
#include <FunctionLayer/Texture/ConstantTexture.h>

PhongMaterial::PhongMaterial() {
  // default
  albedo = std::make_shared<ConstantTexture<Spectrum>>(Spectrum(.5f));
}

PhongMaterial::PhongMaterial(const Json &json) : Material(json) {
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

  kd = fetchRequired<float>(json, "kd");
  ks = fetchRequired<float>(json, "ks");
  p = fetchRequired<float>(json, "p");
}

std::shared_ptr<BSDF>
PhongMaterial::computeBSDF(const Intersection &intersection) const {
  Vector3f normal, tangent, bitangent;
  computeShadingGeometry(intersection, &normal, &tangent, &bitangent);

  Spectrum s = albedo->evaluate(intersection);
  return std::make_shared<PhongReflection>(normal, tangent, bitangent, s, kd,
                                           ks, p);
}

REGISTER_CLASS(PhongMaterial, "phong")