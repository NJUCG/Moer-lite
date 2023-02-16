#pragma once
#include <CoreLayer/ColorSpace/Spectrum.h>
#include <FunctionLayer/Ray/Ray.h>
#include <FunctionLayer/Shape/Intersection.h>
#include <ResourceLayer/JsonUtil.h>

enum class LightType { SpotLight, AreaLight, EnvironmentLight };

struct LightSampleResult {
  Spectrum energy;    // 光源的能量
  Vector3f direction; // 光源的采样相对shadingPoint的方向
  float distance;     // 光源的采样相对shadingPoint的距离
  Vector3f normal;    // 光源采样处的法线，可选的
  float pdf;          // 得到该采样的pdf
  bool isDelta;       // 采样是否是一个delta分布
  LightType type;     // 光源的类型
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

public:
  LightType type;
};

//* 位于无穷远处的光源，由于此时不会有Intersection，所以InfiniteLight提供了新的接口
class InfiniteLight : public Light {
public:
  InfiniteLight() = delete;

  InfiniteLight(const Json &json) : Light(json) {}

  //! 该接口不应当被调用
  virtual Spectrum evaluateEmission(const Intersection &intersection,
                                    const Vector3f &wo) const override final {
    std::cerr << "This shouldn't be invoked!\n";
    exit(1);
  }

  virtual Spectrum evaluateEmission(const Ray &ray) const = 0;

  virtual LightSampleResult sample(const Intersection &shadingPoint,
                                   const Vector2f &sample) const override = 0;
};