#pragma once
#include "Intersection.h"
#include <FunctionLayer/Ray/Ray.h>
#include <FunctionLayer/Texture/Texture.h>
#include <ResourceLayer/JsonUtil.h>
#include <embree3/rtcore.h>
#include <optional>

class Shape : public Transformable {
public:
  Shape() = default;

  Shape(const Json &json);

  virtual RTCGeometry getEmbreeGeometry(RTCDevice device) const;

  std::pair<Point3f, Point3f> getAABB() const;

  virtual bool rayIntersectShape(const Ray &ray, float *distance, int *primID,
                                 float *u, float *v) const = 0;

  virtual void fillIntersection(float distance, int primID, float u, float v,
                                Intersection *intersection) const = 0;

  int geometryID;

protected:
  Point3f pMin, pMax; // AABB包围盒，构建时初始化
};