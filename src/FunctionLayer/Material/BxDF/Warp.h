#pragma once
#include <CoreLayer/Math/Math.h>

inline Vector3f squareToUniformHemisphere(Vector2f sample) {
  float y = 1 - 2 * sample[0];
  float r = fm::sqrt(std::max(.0f, 1.f - y * y));
  float phi = 2 * PI * sample[1];
  return {r * fm::sin(phi), std::abs(y), r * fm::cos(phi)};
}

inline float squareToUniformHemispherePdf(Vector3f v) {
  return v[1] >= .0f ? INV_PI * .5f : .0f;
}
