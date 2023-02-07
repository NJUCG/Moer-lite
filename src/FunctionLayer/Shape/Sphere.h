#pragma once
#include "Shape.h"
// TODO 当前只有Transform中的translate对sphere生效
class Sphere : public Shape {
public:
  Sphere() = delete;

  Sphere(const Json &json);

  virtual RTCGeometry getEmbreeGeometry(RTCDevice device) const override;

  Point3f center;
  float radius;
};