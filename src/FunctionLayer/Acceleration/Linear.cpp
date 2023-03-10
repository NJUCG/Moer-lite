#include "Linear.h"

LinearAcceleration::LinearAcceleration() {
  // do nothing
}

void LinearAcceleration::build() {
  for (auto shape : shapes)
    shape->initInternalAcceleration();
}

bool LinearAcceleration::rayIntersect(Ray &ray, int *geomID, int *primID,
                                      float *u, float *v) const {
  // Just traverse all shapes in the scene

  for (const auto shape : shapes) {
    if (shape->rayIntersectShape(ray, primID, u, v)) {
      *geomID = shape->geometryID;
    }
  }
  return (*geomID != -1);
}