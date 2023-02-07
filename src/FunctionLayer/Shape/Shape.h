#pragma once
#include "Intersection.h"
#include <FunctionLayer/Ray/Ray.h>
#include <ResourceLayer/JsonUtil.h>
#include <embree3/rtcore.h>
class Shape : public Transformable {
public:
  Shape() = default;

  Shape(const Json &json) {
    Vector3f translate =
        fetchOptional(json["transform"], "translate", Vector3f{0.f, 0.f, 0.f});
    Vector3f scale =
        fetchOptional(json["transform"], "scale", Vector3f{1.f, 1.f, 1.f});

    Matrix4f translateMat = Transform::translation(translate);
    Matrix4f scaleMat = Transform::scalation(scale);
    Matrix4f rotateMat = Matrix4f::identity();
    if (json["transform"].contains("rotate")) {
      Vector3f axis =
          fetchRequired<Vector3f>(json["transform"]["rotate"], "axis");
      float radian =
          fetchRequired<float>(json["transform"]["rotate"], "radian");
      rotateMat = Transform::rotation(axis, radian);
    }
    transform = Transform{translateMat, rotateMat, scaleMat};
  }

  virtual RTCGeometry getEmbreeGeometry(RTCDevice device) const = 0;

  int geometryID;
};