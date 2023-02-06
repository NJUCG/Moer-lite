#pragma once
#include "Shape.h"
#include <ResourceLayer/Mesh.h>
class Triangle : public Shape {
public:
  Triangle() = default;

  Triangle(const Transform &_transform, std::shared_ptr<Mesh> _mesh);

  virtual RTCGeometry getEmbreeGeometry(RTCDevice device) const;

private:
  std::shared_ptr<Mesh> mesh;
};