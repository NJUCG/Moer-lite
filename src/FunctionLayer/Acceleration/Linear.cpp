#include "Linear.h"

LinearAcceleration::LinearAcceleration() {
  // do nothing
}

void LinearAcceleration::build() {
  // do nothing
}

std::optional<Intersection> LinearAcceleration::rayIntersect(Ray &ray) const {
  // Just traverse all shapes in the scene
  float u, v;
  int primID, geomID = -1;
  for (const auto shape : shapes) {
    if (shape->rayIntersectShape(ray, &primID, &u, &v)) {
      geomID = shape->geometryID;
    }
  }
  if (geomID == -1)
    return std::nullopt;
  Intersection its;
  shapes[geomID]->fillIntersection(ray.tFar, primID, u, v, &its);
  return std::make_optional(its);
}