#pragma once
#include "BSDF.h"
#include "CoreLayer/ColorSpace/Spectrum.h"
#include "ResourceLayer/JsonUtil.h"
#include "Warp.h"
#include <ResourceLayer/JsonUtil.h>
#include <vector>

inline Spectrum color_from_vec(std::vector<double> arr) {
  auto r = arr[0], g = arr[1], b = arr[2];
  return SpectrumRGB((float)r, (float)g, (float)b);
}

#define READ(name, default)                                                    \
  do {                                                                         \
    name = fetchOptional(config, #name, default);                              \
  } while (0)

#define READ_COLOR(name, default_r, default_g, default_b)                      \
  do {                                                                         \
    name = color_from_vec(fetchOptional(                                       \
        config, #name, std::vector({default_r, default_g, default_b})));       \
  } while (0)

class DisneyBSDF : public BSDF {
public:
  DisneyBSDF(const Json &config) {
    READ_COLOR(base_color, 0.4, 0.7, 0.2);
    READ(subsurface, 0.0);
    READ(metallic, 0.5);
    READ(specular, 0.5);
    READ_COLOR(specular_tint, 0.9, 0.9, 0.0);
  }

private:
  Spectrum base_color, specular_tint, sheen_tint;
  float subsurface, metallic, specular, roughness, anisotropic, sheen,
      clearcoat, clearcoat_roughness, ior, transmission, transmission_roughness;
}
