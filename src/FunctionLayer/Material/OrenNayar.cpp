#include "OrenNayar.h"
#include "BxDF/OrenNayar.h"
#include <FunctionLayer/Texture/ConstantTexture.h>

OrenNayarMaterial::OrenNayarMaterial() {
  // default
  albedo = std::make_shared<ConstantTexture<Spectrum>>(Spectrum(.5f));
}

OrenNayarMaterial::OrenNayarMaterial(const Json &json) : Material(json) {
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

  roughness = fetchRequired<float>(json, "roughness");
}

std::shared_ptr<BSDF>
OrenNayarMaterial::computeBSDF(const Intersection &intersection) const {
  Vector3f normal, tangent, bitangent;
  computeShadingGeometry(intersection, &normal, &tangent, &bitangent);

  Spectrum s = albedo->evaluate(intersection);
  return std::make_shared<OrenNayarBSDF>(normal, tangent, bitangent, s,
                                         roughness);
}

REGISTER_CLASS(OrenNayarMaterial, "oren-nayar")