#pragma once
#include "BSDF.h"
#include "CoreLayer/ColorSpace/RGB.h"
#include "CoreLayer/ColorSpace/Spectrum.h"
#include "CoreLayer/Math/Constant.h"
#include "CoreLayer/Math/Geometry.h"

#include "ResourceLayer/JsonUtil.h"
#include "Warp.h"
#include <ResourceLayer/JsonUtil.h>
#include <__math/logarithms.h>
#include <__math/roots.h>
#include <algorithm>
#include <cstdlib>

#include <pthread.h>
#include <vector>

// The Disney BSDF model
// Reference: https://cseweb.ucsd.edu/~tzli/cse272/wi2023/homework1.pdf

inline Spectrum color_from_vec(std::vector<double> arr) {
  auto r = arr[0], g = arr[1], b = arr[2];
  return Spectrum((float)r, (float)g, (float)b);
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

// When the ray is inside the object, just return 0 (or color 0)
// This should be used for diffuse, metal, clearcoat and sheen
#define HANDLE_INSIDE_OBJ(ret_val)                                             \
  do {                                                                         \
    if (dot(wi, normal) <= 0) {                                                \
      /*return ret_val;*/                                                      \
    }                                                                          \
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

protected:
  auto f_diffuse(const Vector3f &wi, const Vector3f &wo,
                 const Vector3f &h) const {
    HANDLE_INSIDE_OBJ(Spectrum(0.0));
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
    return (1.0 - subsurface) * f_base_diffuse + subsurface * f_subsurface;
  }

  auto f_metal(const Vector3f &wi, const Vector3f &wo,
               const Vector3f &h) const {
    HANDLE_INSIDE_OBJ(Spectrum(0.0));
    // Fm
    // auto Fm = base_color +
    //           (SpectrumRGB(1.0) - base_color) * pow(1.0 - adot(h, wo), 5);

    // When combining them together the PDF says we should modify the metal
    // function a bit
    auto base_luminance = base_color.luminance();
    auto Ctint = base_luminance > 0 ? base_color / base_luminance : 1;
    auto Ks = (Spectrum(1.0) - specular_tint) + specular_tint * Ctint;
    auto R0 = [](auto eta) {
      auto upper = pow(eta - 1.0, 2);
      auto lower = pow(eta + 1.0, 2);
      return upper / lower;
    };
    auto C0 =
        specular * R0(ior) * (1.0 - metallic) * Ks + metallic * base_color;
    auto Fm = C0 + (Spectrum(1.0) - C0) * pow(1 - dot(h, wo), 5);

    // Dm
    // UCSD Homework's Dm sometimes can be very near to 0
    // and looks trange. Using formula from ChatGPT.
    auto aspect = sqrt(1.0 - 0.9 * anisotropic);
    auto alpha_min = 1e-4;
    auto alpha_x = std::max(alpha_min, roughness * roughness / aspect);
    auto alpha_y = std::max(alpha_min, roughness * roughness * aspect);
    auto hl = normalize(toLocal(h));
    // auto Dm =
    //     1.0 /
    //     (PI * alpha_x * alpha_y *
    //      pow(pow(hl[0] / alpha_x, 2) + pow(hl[1] / alpha_y, 2) + pow(hl[2],
    //      2),
    //          2));
    auto alpha = pow(roughness, 2);
    auto Dm = pow(alpha, 2) /
              (PI * pow(pow(dot(normal, h), 2) * (alpha * alpha - 1) + 1, 2));

    // Gm
    auto A = [this, alpha_x, alpha_y](Vector3f w) {
      auto wl = toLocal(w);
      auto sqrt_inner_upper = pow(wl[0] * alpha_x, 2) + pow(wl[1] * alpha_y, 2);
      auto sqrt_inner = 1.0 + sqrt_inner_upper / pow(wl[2], 2);
      auto upper = sqrt(sqrt_inner) - 1.0;
      return upper / 2.0;
    };
    auto G = [A](auto w) { return 1.0 / (1.0 + A(w)); };
    auto Gm = G(wi) * G(wo);

    return Fm * Dm * Gm / (4 * adot(normal, wi));
  }

  auto f_clearcoat(const Vector3f &wi, const Vector3f &wo,
                   const Vector3f &h) const {
    HANDLE_INSIDE_OBJ(0.0);
    auto A = [this](Vector3f w) {
      auto wl = toLocal(w);
      auto sqrt_inner_upper = pow(wl[0] * 0.25, 2) + pow(wl[1] * 0.25, 2);
      auto sqrt_inner = 1.0 + sqrt_inner_upper / pow(wl[2], 2);
      auto upper = sqrt(sqrt_inner) - 1.0;
      return upper / 2.0;
    };
    auto G = [A](auto w) { return 1.0 / (1.0 + A(w)); };
    auto Gc = G(wi) * G(wo);

    // 原公式使用clearcoat gloss，它等于1-clearcoat_roughness
    auto alpha_g =
        clearcoat_roughness * 0.1 + (1.0 - clearcoat_roughness) * 0.001;
    auto alpha_g2 = pow(alpha_g, 2);
    auto hl = toLocal(h);
    auto Dc = (alpha_g2 - 1.0) /
              (PI * log(alpha_g2) * (1.0 + (alpha_g2 - 1.0) * pow(hl[2], 2)));

    auto R0 = [](auto eta) {
      auto upper = pow(eta - 1.0, 2);
      auto lower = pow(eta + 1.0, 2);
      return upper / lower;
    };
    auto R0_15 = R0(1.5);
    auto Fc = R0_15 + (1 - R0_15) * pow(1 - adot(h, wo), 5);

    return Fc * Dc * Gc / (4 * adot(normal, wi));
  }

  auto f_glass(const Vector3f &wi, const Vector3f &wo,
               const Vector3f &h) const {
    auto condition = dot(normal, wi) * dot(normal, wo);

    // Fg
    auto hwin = dot(h, wi), hwout = dot(h, wo);
    auto Rs = (hwin - ior * hwout) / (hwin + ior * hwout);
    auto Rp = (ior * hwin - hwout) / (ior * hwin + hwout);
    auto Fg = 0.5 * (pow(Rs, 2) + pow(Rp, 2));

    // Dg
    auto aspect = sqrt(1.0 - 0.9 * anisotropic);
    auto alpha_min = 1e-5;
    auto alpha_x = std::max(alpha_min, roughness * roughness / aspect);
    auto alpha_y = std::max(alpha_min, roughness * roughness * aspect);
    auto hl = toLocal(h);
    auto Dg =
        1.0 /
        (PI * alpha_x * alpha_y *
         pow(pow(h[0] / alpha_x, 2) + pow(h[1] / alpha_y, 2) + pow(h[2], 2),
             2));

    // Gg
    auto A = [this, alpha_x, alpha_y](Vector3f w) {
      auto wl = toLocal(w);
      auto sqrt_inner_upper = pow(w[0] * alpha_x, 2) + pow(w[1] * alpha_y, 2);
      auto sqrt_inner = 1.0 + sqrt_inner_upper / pow(w[2], 2);
      auto upper = sqrt(sqrt_inner) - 1.0;
      return upper / 2.0;
    };
    auto G = [A](auto w) { return 1.0 / (1.0 + A(w)); };
    auto Gg = G(wi) * G(wo);

    if (condition > 0) {
      auto upper = base_color * Fg * Dg * Gg;
      auto lower = 4.0 * adot(normal, wi);
      return upper / lower;
    } else {
      auto upper = base_color.sqrt_color() * (1.0 - Fg) * Dg * Gg *
                   abs(dot(h, wo) * dot(h, wi));
      auto lower = adot(normal, wi) * pow(dot(h, wi) + ior * dot(h, wo), 2);
      return upper / lower;
    }
  }

  auto f_sheen(const Vector3f &wi, const Vector3f &wo,
               const Vector3f &h) const {
    HANDLE_INSIDE_OBJ(Spectrum(0.0));
    auto base_luminance = base_color.luminance();
    auto Ctint = base_luminance > 0 ? base_color / base_luminance : 1;
    auto Csheen = (Spectrum(1.0) - sheen_tint) + sheen_tint * Ctint;

    return Csheen * pow(1 - adot(h, wo), 5) * adot(normal, wo);
  }

  auto f_disney(const Vector3f &wi, const Vector3f &wo) const {
    // half vector
    auto h = (wi + wo) / (wi + wo).length();
    return (1.0 - specular) * (1.0 - metallic) * f_diffuse(wi, wo, h) +
           (1.0 - metallic) * sheen * f_sheen(wi, wo, h) +
           (1.0 - specular * (1.0 - metallic)) * f_metal(wi, wo, h) +
           0.25 * clearcoat * f_clearcoat(wi, wo, h) +
           (1.0 - metallic) * specular * f_glass(wi, wo, h);
    // return f_sheen(wi, wo, h);
    // return Spectrum(0.01);
  }

public:
  // 关于sample函数和f函数的返回值，参考以下对话：
  // https://openwebui.com/c/kken/ffdeae08-2c35-4c25-bb39-ef5c270b8828

  // wo: 从交点去到相机的方向（与ucsd作业是相反的）
  // 但是反正BSDF是对称的，wo和wi对调一下不影响结果
  virtual BSDFSampleResult sample(const Vector3f &wo,
                                  const Vector2f &sample) const override {
    auto wi = squareToCosineHemisphere(sample);
    auto pdf = squareToCosineHemispherePdf(wi);

    return {f_disney(wi, wo) * PI, toWorld(wi), pdf, BSDFType::Diffuse};
  }

  virtual Spectrum f(const Vector3f &wo, const Vector3f &wi) const override {
    auto cos_theta = toLocal(wi)[1];
    return f_disney(wi, wo) * std::max(cos_theta, 0.0f);
  }

private:
  Spectrum base_color, specular_tint, sheen_tint;
  float subsurface, metallic, specular, roughness, anisotropic, sheen,
      clearcoat, clearcoat_roughness, ior, transmission, transmission_roughness;

  Matrix4f x;
};
