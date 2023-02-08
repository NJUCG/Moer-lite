#pragma once
#include "Shape.h"

//* 平行四边形
class Parallelogram : public Shape {
public:
  Parallelogram() = delete;

  Parallelogram(const Json &json);

  virtual RTCGeometry getEmbreeGeometry(RTCDevice device) const override;

  virtual Vector2f getUVTexcod(int primID, float u, float v) const override;
  Point3f base;
  Vector3f edge0, edge1;
};