#pragma once
#include "BSDF.h"
#include "CoreLayer/ColorSpace/RGB.h"
#include "CoreLayer/ColorSpace/Spectrum.h"
#include "CoreLayer/Math/Constant.h"
#include "CoreLayer/Math/Geometry.h"
#include "ResourceLayer/JsonUtil.h"
#include "Warp.h"
#include <ResourceLayer/JsonUtil.h>
#include <__math/roots.h>
#include <algorithm>
#include <cstdlib>
#include <pthread.h>
#include <vector>

// The Disney BSDF model
// Reference: https://cseweb.ucsd.edu/~tzli/cse272/wi2023/homework1.pdf

inline Spectrum color_from_vec(std::vector<double> arr) {
  auto r = arr[0], g = arr[1], b = arr[2];
  return SpectrumRGB((float)r, (float)g, (float)b);
}

// abs dot
auto adot(const Vector3f &a, const Vector3f &b) { return abs(dot(a, b)); }

// Read float from json config
#define READ(name, default)                                                    \
  do {                                                                         \
    name = fetchOptional(config, #name, default);                              \
  } while (0)

// Read color from json config
#define READ_COLOR(name, default_r, default_g, default_b)                      \
  do {                                                                         \
    name = color_from_vec(fetchOptional(                                       \
        config, #name, std::vector({default_r, default_g, default_b})));       \
  } while (0)

class DisneyBSDF : public BSDF {
public:
  DisneyBSDF(const Json &config, const Vector3f &normal,
             const Vector3f &tangent, const Vector3f &bitangent)
      : BSDF(normal, tangent, bitangent) {
    READ_COLOR(base_color, 0.4, 0.7, 0.2);
    READ(subsurface, 0.0);
    READ(metallic, 0.5);
    READ(specular, 0.5);
    READ_COLOR(specular_tint, 0.9, 0.9, 0.0);
    READ(roughness, 0.5);
    READ(anisotropic, 1.0);
    READ(sheen, 0.1);
    READ_COLOR(sheen_tint, 0.8, 0.8, 0.6);
    READ(clearcoat, 0.7);
    READ(clearcoat_roughness, 0.1);
    READ(ior, 0.3);
    READ(transmission, 0.1);
    READ(transmission_roughness, 0.1);
  }

  // wo: 从交点去到相机的方向（与ucsd作业是相反的）
  // 但是反正BSDF是对称的，wo和wi对调一下不影响结果
  virtual BSDFSampleResult sample(const Vector3f &wo,
                                  const Vector2f &sample) const override {
    auto wi = squareToCosineHemisphere(sample);
    auto pdf = squareToCosineHemispherePdf(wi);

    // half vector
    auto h = (wi + wo) / (wi + wo).length();
    // the cosine term
    auto ct = adot(normal, wo);

    return {/*todo */, toWorld(wi), pdf, BSDFType::Diffuse};
  }

private:
  Spectrum base_color, specular_tint, sheen_tint;
  float subsurface, metallic, specular, roughness, anisotropic, sheen,
      clearcoat, clearcoat_roughness, ior, transmission, transmission_roughness;

protected:
  auto diffuse(const Vector3f &wi, const Vector3f &wo, const Vector3f &h,
               float ct) const {
    // Base diffuse
    auto fd90 = 0.5 + 2.0 * roughness * pow(dot(h, wo), 2);
    auto fd = [this, fd90](auto w) {
      return 1.0 + (fd90 - 1) * pow(1.0 - adot(this->normal, w), 5);
    };

    auto f_base_diffuse = base_color / PI * fd(wi) * fd(wo) * adot(normal, wo);

    // Subsurface
    auto fss90 = roughness * pow(dot(h, wo), 2);
    auto fss = [this, fss90](auto w) {
      return 1.0 + (fss90 - 1.0) * pow(1.0 - adot(normal, w), 5);
    };

    auto f_subsurface =
        1.25 * base_color / PI *
        (fss(wi) * fss(wo) *
             (1.0 / (adot(normal, wi) + adot(normal, wo)) - 0.5) +
         0.5) *
        adot(normal, wo);

    // Combine them
    auto diffuse =
        (1.0 - subsurface) * f_base_diffuse + subsurface * f_subsurface;
    return diffuse;
  }

  auto metal(const Vector3f &wi, const Vector3f &wo, const Vector3f &h,
             float ct) const {
    // Fm
    auto fm =
        base_color + (SpectrumRGB(1.0) - base_color) * pow(1 - adot(h, wo), 5);

    // Dm
    auto aspect = sqrt(1 - 0.9 * anisotropic);
    auto alpha_min = 1e-5;
    auto alpha_x = std::max(alpha_min, roughness * roughness / aspect);
    auto alpha_y = std::max(alpha_min, roughness * roughness * aspect);
    auto dm = 1 / (PI * alpha_x * alpha_y *
                   (

                       ));
  }
}
