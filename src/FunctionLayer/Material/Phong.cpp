#include "Phong.h"
#include "./BxDF/Phong.h"
#include <FunctionLayer/Texture/ConstantTexture.h>

PhongMaterial::PhongMaterial() {
  // default
  kd = std::make_shared<ConstantTexture<Spectrum>>(Spectrum(.5f));
}

PhongMaterial::PhongMaterial(const Json &json) : Material(json) {
  if (json["kd"].is_object()) {
    kd = Factory::construct_class<Texture<Spectrum>>(json["kd"]);
  } else if (json["kd"].is_array()) {
    auto s = fetchRequired<Spectrum>(json, "kd");
    kd = std::make_shared<ConstantTexture<Spectrum>>(s);
  } else {
    std::cerr << "Error in kd format!\n";
    exit(1);
  }
  if (json["ks"].is_object()) {
    ks = Factory::construct_class<Texture<Spectrum>>(json["ks"]);
  } else if (json["ks"].is_array()) {
    auto s = fetchRequired<Spectrum>(json, "ks");
    ks = std::make_shared<ConstantTexture<Spectrum>>(s);
  } else {
    std::cerr << "Error in ks format!\n";
    exit(1);
  }
  p = fetchRequired<float>(json, "p");
}

std::shared_ptr<BSDF>
PhongMaterial::computeBSDF(const Intersection &intersection) const {
  Vector3f normal, tangent, bitangent;
  computeShadingGeometry(intersection, &normal, &tangent, &bitangent);

  Spectrum _kd = kd->evaluate(intersection);
  Spectrum _ks = ks->evaluate(intersection);
  return std::make_shared<PhongReflection>(normal, tangent, bitangent, _kd, _ks,
                                           p);
}

REGISTER_CLASS(PhongMaterial, "phong")