#pragma once
#include "../NDF/NDF.h"
#include "BSDF.h"
#include "Warp.h"

class RoughDielectricBSDF : public BSDF {
public:
  RoughDielectricBSDF(const Vector3f &_normal, const Vector3f &_tangent,
                      const Vector3f &_bitangent, Spectrum _albedo,
                      Vector2f _alpha, float _eta, NDF *_ndf)
      : BSDF(_normal, _tangent, _bitangent), albedo(_albedo), alpha(_alpha),
        eta(_eta), ndf(_ndf) {}

  virtual Spectrum f(const Vector3f &wo, const Vector3f &wi) const override {
    // TODO
    // 1. 转换坐标系到局部坐标
    // 2. 根据公式计算 Fr, D, G
    // 3. return albedo * D * G * Fr / (4 * \cos\theta_o);
    // tips:
    // 不考虑多重介质，如果光线从真空射入介质，其eta即配置中填写的eta；
    // 如果光线从介质射出，则eta = 1/eta
    return {0.f};
  }

  virtual BSDFSampleResult sample(const Vector3f &wo,
                                  const Vector2f &sample) const override {
    Vector3f wi = squareToCosineHemisphere(sample);
    float pdf = squareToCosineHemispherePdf(wi);
    return {f(wo, toWorld(wi)) / pdf, toWorld(wi), pdf, BSDFType::Diffuse};
  }

  float getR0(float etaO) const noexcept {
    return ((etaO - 1.f) * (etaO - 1.f)) / ((etaO + 1.f) * (etaO + 1.f));
  }
  float getFr(float etaO, float cosTheta) const noexcept {
    float r0 = getR0(etaO);
    return r0 + (1.f - r0) * std::pow(1.f - cosTheta, 5.f);
  }

private:
  Spectrum albedo;
  Vector2f alpha;
  float eta;
  NDF *ndf;
};