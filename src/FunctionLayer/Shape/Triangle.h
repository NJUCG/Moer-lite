#pragma once
#include "Shape.h"
#include <ResourceLayer/Factory.h>
#include <ResourceLayer/Mesh.h>
class Triangle : public Shape {
public:
  Triangle() = default;

  Triangle(const Json &json);

  //* 当使用embree时，我们使用embree内置的求交函数，故覆盖默认方法
  virtual RTCGeometry getEmbreeGeometry(RTCDevice device) const override;

  virtual bool rayIntersectShape(const Ray &ray, float *distance, int *primID,
                                 float *u, float *v) const override;

  virtual void fillIntersection(float distance, int primID, float u, float v,
                                Intersection *intersection) const override;

private:
  std::shared_ptr<Mesh> mesh;
};