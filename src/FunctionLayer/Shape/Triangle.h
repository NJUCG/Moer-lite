#pragma once
#include "Shape.h"
#include <ResourceLayer/Factory.h>
#include <ResourceLayer/Mesh.h>
class Triangle : public Shape {
public:
  Triangle() = default;

  Triangle(const Json &json);

  virtual RTCGeometry getEmbreeGeometry(RTCDevice device) const;

private:
  std::shared_ptr<Mesh> mesh;
};