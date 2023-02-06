#pragma once
#include "Intersection.h"
#include <FunctionLayer/Ray/Ray.h>
#include <ResourceLayer/JsonUtil.h>
#include <embree3/rtcore.h>
class Shape : public Transformable {
public:
  Shape() = default;

  Shape(const Transform &_transform) : Transformable(_transform) {}

  virtual RTCGeometry getEmbreeGeometry(RTCDevice device) const = 0;
};