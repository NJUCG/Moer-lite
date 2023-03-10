#pragma once
#include "Shape.h"

//* 平行四边形
class Parallelogram : public Shape {
public:
  Parallelogram() = delete;

  Parallelogram(const Json &json);

  virtual bool rayIntersectShape(Ray &ray, int *primID, float *u,
                                 float *v) const override;

  virtual void fillIntersection(float distance, int primID, float u, float v,
                                Intersection *intersection) const override;

  virtual void uniformSampleOnSurface(Vector2f sample,
                                      Intersection *intersection,
                                      float *pdf) const override;

public:
  Point3f base;
  Vector3f edge0, edge1;
};