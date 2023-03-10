#pragma once
#include "Acceleration.h"
#include <embree3/rtcore.h>

//* embree是CPU端性能最好的光线求交引擎，其内置的加速结构是BVH
//* 关于embree的资料可以在https://www.embree.org/中查找
//* embree在github上的地址https://github.com/embree/embree
class EmbreeBVH : public Acceleration {
public:
  EmbreeBVH();

  virtual ~EmbreeBVH() = default;

  virtual void build() override;

  virtual bool rayIntersect(Ray &ray, int *geomID, int *primID, float *u,
                            float *v) const override;

private:
  //* 通过RTCDevice和RTCScene调用embree
  RTCDevice device;
  RTCScene scene;
};