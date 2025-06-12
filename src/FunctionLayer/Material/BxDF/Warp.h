#pragma once
#include <CoreLayer/Math/Math.h>

// 均匀地将正方形map到半球面
inline Vector3f squareToUniformHemisphere(Vector2f sample) {
  float y = 1 - 2 * sample[0];
  float r = fm::sqrt(std::max(.0f, 1.f - y * y));
  float phi = 2 * PI * sample[1];
  Vector3f dir{r * fm::sin(phi), std::abs(y), r * fm::cos(phi)};
  return normalize(dir);
}

// 均匀地将正方形map到半球面的概率密度函数
inline float squareToUniformHemispherePdf(Vector3f v) {
  return v[1] >= .0f ? INV_PI * .5f : .0f;
}

// 余弦加权采样。半球面上，theta夹角（与法线夹角）的cos值为pdf值。
inline Vector3f squareToCosineHemisphere(Vector2f sample) {
  float phi = 2 * M_PI * sample[0], theta = std::acos(std::sqrt(sample[1]));
  return Vector3f{std::sin(theta) * std::sin(phi), std::cos(theta),
                  std::sin(theta) * std::cos(phi)};
}

// 余弦加权采样的pdf。
inline float squareToCosineHemispherePdf(Vector3f v) {
  return (v[1] > .0f) ? v[1] * INV_PI : .0f;
}
