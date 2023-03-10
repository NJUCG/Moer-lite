#pragma once
#include "Acceleration.h"

//* Linear结构对所有shape做线性遍历，因此没有任何求交优化
//* 线性遍历的计算开销通常是无法接受的
class LinearAcceleration : public Acceleration {
public:
  LinearAcceleration();

  virtual ~LinearAcceleration() = default;

  virtual void build() override;

  virtual bool rayIntersect(Ray &ray, int *geomID, int *primID, float *u,
                            float *v) const override;
};