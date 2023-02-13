#pragma once
#include <CoreLayer/ColorSpace/Spectrum.h>
#include <FunctionLayer/Ray/Ray.h>
#include <FunctionLayer/Shape/Intersection.h>
#include <ResourceLayer/JsonUtil.h>

enum class LightType { SpotLight, AreaLight, EnvironmentLight };

struct LightSampleResult {
  Spectrum emission;
  Point3f position;
  Vector3f normal; // 可选的
  float pdf;
  bool isDelta;
  LightType type;
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