#pragma once
#include <CoreLayer/Math/Math.h>

inline Vector3f squareToUniformHemisphere(Vector2f sample) {
  float y = 1 - 2 * sample[0];
  float r = fm::sqrt(std::max(.0f, 1.f - y * y));
  float phi = 2 * PI * sample[1];
  Vector3f dir{r * fm::sin(phi), std::abs(y), r * fm::cos(phi)};
  return normalize(dir);
}

inline float squareToUniformHemispherePdf(Vector3f v) {
  return v[1] >= .0f ? INV_PI * .5f : .0f;
}

inline Vector3f squareToCosineHemisphere(Vector2f sample) {
  float phi = 2 * M_PI * sample[0], theta = std::acos(std::sqrt(sample[1]));
  return Vector3f{std::sin(theta) * std::sin(phi), std::cos(theta),
                  std::sin(theta) * std::cos(phi)};
}

inline float squareToCosineHemispherePdf(Vector3f v) {
  return (v[1] > .0f) ? v[1] * INV_PI : .0f;
}
