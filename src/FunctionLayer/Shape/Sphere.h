#pragma once
#include "Shape.h"
// TODO 当前只有Transform中的translate对sphere生效
class Sphere : public Shape {
public:
  Sphere() = delete;

  Sphere(const Json &json);

  virtual bool rayIntersectShape(const Ray &ray, float *distance, int *primID,
                                 float *u, float *v) const override;

  virtual void fillIntersection(float distance, int primID, float u, float v,
                                Intersection *intersection) const override;
  // TODO finish this
  virtual void uniformSampleOnSurface(Vector2f sample,
                                      Intersection *intersection,
                                      float *pdf) const override {
    //
  }

public:
  Point3f center;
  float radius;
};