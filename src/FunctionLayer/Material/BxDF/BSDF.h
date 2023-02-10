#pragma once
#include <CoreLayer/ColorSpace/Spectrum.h>
#include <FunctionLayer/Shape/Intersection.h>
struct BSDFSampleResult {
  Spectrum weight;
  Vector3f wi;
  float pdf;
};

class BSDF {
public:
  BSDF(const Intersection &intersection) {
    normal = intersection.normal;
    tangent = intersection.tangent;
    bitangent = intersection.bitangent;
  }
  virtual Spectrum f(const Vector3f &wo, const Vector3f &wi) const = 0;
  virtual BSDFSampleResult sample(const Vector3f &wo,
                                  const Vector2f &sample) const = 0;

protected:
  Vector3f toLocal(Vector3f world) const {
    return Vector3f{dot(tangent, world), dot(normal, world),
                    dot(bitangent, world)};
  }
  Vector3f toWorld(Vector3f local) const {
    return local[0] * tangent + local[1] * normal + local[2] * bitangent;
  }

  Vector3f normal, tangent, bitangent; // 构成局部坐标系
};