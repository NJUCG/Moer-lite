#pragma once
#include <CoreLayer/ColorSpace/Spectrum.h>
#include <FunctionLayer/Shape/Intersection.h>
#include <ResourceLayer/JsonUtil.h>
struct LightSampleResult {
  Spectrum emission;
  Point3f position;
  Vector3f normal; // 可选的
  float pdf;
  bool isDelta;
};

class Light {
public:
  Light() = delete;

  Light(const Json &json) {}

  //* 当一条光线击中光源时，返回光源沿光线方向释放的radiance
  //* intersection是光线和光源的交点
  virtual Spectrum evaluateEmission(const Intersection &intersection,
                                    const Vector3f &wo) const = 0;
  //* 在shadingPoint处对光源进行采样，返回采样结果
  virtual LightSampleResult sample(const Intersection &shadingPoint,
                                   const Vector2f &sample) const = 0;
};