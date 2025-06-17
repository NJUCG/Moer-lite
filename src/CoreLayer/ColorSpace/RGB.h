#pragma once
#include "CoreLayer/Math/Geometry.h"
#include <CoreLayer/Math/Math.h>
#include <math.h>
//* 使用RGB三通道颜色空间

class SpectrumRGB {
public:
  SpectrumRGB() : rgb(0.f) {}

  SpectrumRGB(float f) : rgb(f) {}

  SpectrumRGB(float r, float g, float b) : rgb(r, g, b) {}

  SpectrumRGB(Vector3f _rgb) : rgb(_rgb) {}

  SpectrumRGB operator+(const SpectrumRGB &rhs) const {
    return SpectrumRGB(rgb + rhs.rgb);
  }

  SpectrumRGB &operator+=(const SpectrumRGB &rhs) {
    rgb += rhs.rgb;
    return *this;
  }

  SpectrumRGB operator-(const SpectrumRGB &rhs) const {
    return SpectrumRGB(rgb - rhs.rgb);
  }

  SpectrumRGB &operator-=(const SpectrumRGB &rhs) {
    rgb -= rhs.rgb;
    return *this;
  }

  SpectrumRGB operator*(const SpectrumRGB &rhs) const {
    return SpectrumRGB(rgb * rhs.rgb);
  }

  SpectrumRGB &operator*=(const SpectrumRGB &rhs) {
    rgb *= rhs.rgb;
    return *this;
  }

  SpectrumRGB operator*(float f) const { return SpectrumRGB(rgb * f); }

  SpectrumRGB &operator*=(float f) {
    rgb *= f;
    return *this;
  }

  SpectrumRGB operator/(const SpectrumRGB &rhs) const {
    return SpectrumRGB(rgb / rhs.rgb);
  }

  SpectrumRGB &operator/=(const SpectrumRGB &rhs) {
    rgb /= rhs.rgb;
    return *this;
  }

  SpectrumRGB operator/(float f) const { return SpectrumRGB(rgb / f); }

  SpectrumRGB &operator/=(float f) {
    rgb /= f;
    return *this;
  }

  auto sqrt_color() const {
    return SpectrumRGB(sqrt(rgb[0]), sqrt(rgb[1]), sqrt(rgb[2]));
  }

  auto luminance() const {
    auto C = [](auto x) {
      if (x <= 04045)
        return x / 12.92;
      else {
        return pow((x + 0.055) / 1.055, 2.4);
      }
    };

    auto linear_rgb = Vector3f(C(rgb[0]), C(rgb[1]), C(rgb[2]));
    auto Y = 0.2126 * linear_rgb[0] + 0.7152 * linear_rgb[1] +
             0.0722 * linear_rgb[2];

    return Y;
  }

  float operator[](int i) const { return rgb[i]; }

  float &operator[](int i) { return rgb[i]; }

  bool isZero() const { return rgb.isZero(); }

  void debugPrint() const {
    printf("[rgb](");
    for (int i = 0; i < 3; ++i) {
      std::cout << (i == 0 ? '\0' : ',') << rgb[i];
    }
    printf(")%c", '\n');
    fflush(stdout);
  }

private:
  Vector3f rgb;
};

inline SpectrumRGB operator*(float f, const SpectrumRGB &spectrum) {
  return spectrum * f;
}

inline Vector3f toVec3(const SpectrumRGB &spectrum) {
  return Vector3f{spectrum[0], spectrum[1], spectrum[2]};
}

inline SpectrumRGB toSpectrum(const Vector3f &vec) {
  return SpectrumRGB(vec[0], vec[1], vec[2]);
}
