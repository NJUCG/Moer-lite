#include "EnvironmentLight.h"
#include <ResourceLayer/Factory.h>

//* Helper function
Vector2f direction2uv(Vector3f direction) {
  float u, v;
  float cosTheta = direction[1];
  v = std::acos(cosTheta);
  if (std::abs(direction[2]) < 1e-8f) {
    u = (direction[0] > .0f) ? (PI * .5f) : (PI * 1.5f);
  } else {
    float tanPhi = direction[0] / (direction[2] + 1e-8f);
    u = std::atan(tanPhi); // u in [-.5f * PI, .5f * PI]
    if (direction[0] < .0f && direction[2] < .0f) {
      u += PI;
    } else if (direction[0] < .0f && direction[2] > .0f) {
      u += 2 * PI;
    } else if (direction[0] > .0f && direction[2] < .0f) {
      u += PI;
    }
  }
  u *= .5f * INV_PI;
  v *= INV_PI;
  return {u, v};
}

EnvironmentLight::EnvironmentLight(const Json &json) : InfiniteLight(json) {
  if (!json.contains("texture")) {
    std::cerr << "EnvironmentLight must specify texture!\n";
    exit(1);
  }
  type = LightType::EnvironmentLight;
  environmentMap = Factory::construct_class<Texture<Spectrum>>(json["texture"]);

  //* 初始化环境光的能量分布
  int witdth = environmentMap->size[0], height = environmentMap->size[1];
  std::vector<Vector2i> indices;
  indices.reserve(witdth * height);

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < witdth; ++x) {
      indices.emplace_back(Vector2i{x, y});
    }
  }

  auto weightFunction = [tex = environmentMap](Vector2i index) {
    const static float invWidth = 1.f / tex->size[0],
                       invHeight = 1.f / tex->size[1];
    float u = index[0] * invWidth, v = index[1] * invHeight;
    float sinTheta = fm::sin(PI * (index[1] + .5f) * invHeight);
    TextureCoord texcod{Vector2f{u, v}, Vector2f(.0f, .0f), Vector2f(.0f, .0f)};
    auto s = tex->evaluate(texcod);
    return (s[0] * 0.212671f + s[1] * 0.715160f + s[2] * 0.072169f) * sinTheta;
  };

  energyDistribution = Distribution<Vector2i>(indices, weightFunction);
}

Spectrum EnvironmentLight::evaluateEmission(const Ray &ray) const {
  Vector2f uv = direction2uv(ray.direction);
  return environmentMap->evaluate(
      TextureCoord{uv, Vector2f(.0f, .0f), Vector2f(.0f, .0f)});
}

LightSampleResult EnvironmentLight::sample(const Intersection &shadingPoint,
                                           const Vector2f &sample) const {
  const static float invWidth = 1.f / environmentMap->size[0],
                     invHeight = 1.f / environmentMap->size[1];

  float pdf;
  Vector2i index = energyDistribution.sample(sample[0], &pdf);
  float u = index[0] * invWidth, v = index[1] * invHeight;
  float phi = u * 2 * PI, theta = v * PI;

  float x = fm::sin(theta) * fm::sin(phi), y = fm::cos(theta),
        z = fm::sin(theta) * fm::cos(phi);
  TextureCoord texcod{Vector2f{u, v}, Vector2f(.0f, .0f), Vector2f(.0f, .0f)};
  Spectrum energy = environmentMap->evaluate(texcod);
  pdf *= environmentMap->size[0] * environmentMap->size[1] * INV_PI * INV_PI *
         .5f / (fm::sin(theta));

  return {energy,            //
          Vector3f{x, y, z}, // 光源相对shadingPoint的方向
          FLT_MAX,           // 环境光定义在无穷远处
          Vector3f(),
          pdf,
          false,
          type};
}

REGISTER_CLASS(EnvironmentLight, "environmentLight")