#include "Conductor.h"
#include "BxDF/RoughConductor.h"
#include "NDF/Beckmann.h"
#include "NDF/GGX.h"
#include <FunctionLayer/Texture/ConstantTexture.h>

ConductorMaterial::ConductorMaterial() {
  // default
  albedo = std::make_shared<ConstantTexture<Spectrum>>(Spectrum(.5f));
}

ConductorMaterial::ConductorMaterial(const Json &json) : Material(json) {
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

  eta = fetchRequired<Vector3f>(json, "eta");
  k = fetchRequired<Vector3f>(json, "k");
  if (json["roughness"].is_number()) {
    roughness[0] = fetchRequired<float>(json, "roughness");
    roughness[1] = roughness[0];
  } else if (json["roughness"].is_array()) {
    roughness = fetchRequired<Vector2f>(json, "roughness");
  } else {
    std::cerr << "Error in roughness format!\n";
    exit(1);
  }

  if (json.contains("ndf") && json["ndf"] == "ggx") {
    ndf = std::make_unique<GGXDistribution>();
  } else {
    ndf = std::make_unique<BeckmannDistribution>();
  }
}

std::shared_ptr<BSDF>
ConductorMaterial::computeBSDF(const Intersection &intersection) const {
  Vector3f normal, tangent, bitangent;
  computeShadingGeometry(intersection, &normal, &tangent, &bitangent);

  Spectrum s = albedo->evaluate(intersection);
  return std::make_shared<RoughConductorBSDF>(normal, tangent, bitangent, s,
                                              roughness, eta, k, ndf.get());
}

REGISTER_CLASS(ConductorMaterial, "conductor")