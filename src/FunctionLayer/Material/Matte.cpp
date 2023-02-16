#include "Matte.h"
#include "./BxDF/Lambert.h"
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
  Spectrum s = albedo->evaluate(TextureCoord{intersection.texCoord});
  // TODO

  Intersection its = intersection;
  if (normalMap) {
    Spectrum n = normalMap->evaluate(TextureCoord{intersection.texCoord});
    Vector3f normal =
        normalize(Vector3f{n[0], n[1], n[2]} * 2.f - Vector3f{1.f});
    normal = normalize(normal[0] * intersection.tangent +
                       normal[2] * intersection.normal +
                       normal[1] * intersection.bitangent);
    Vector3f tangent = normalize(its.dpdu);
    Vector3f bitangent = normalize(cross(tangent, normal));
    its.normal = normal;
    its.tangent = tangent;
    its.bitangent = bitangent;
  }

  return std::make_shared<LambertReflection>(its, s);
}

REGISTER_CLASS(MatteMaterial, "matte")