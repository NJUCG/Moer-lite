#include "Shape.h"
#include <FunctionLayer/Material/Matte.h>
#include <FunctionLayer/Material/Mirror.h>

Shape::Shape(const Json &json) {
  if (json.contains("transform")) {
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
  if (json.contains("material")) {
    material = Factory::construct_class<Material>(json["material"]);
  } else {
    material = std::make_shared<MatteMaterial>();
  }
}

void UserShapeBound(const RTCBoundsFunctionArguments *args) {
  Shape *shape = static_cast<Shape *>(args->geometryUserPtr);
  auto [pMin, pMax] = shape->getAABB();
  args->bounds_o->lower_x = pMin[0];
  args->bounds_o->lower_y = pMin[1];
  args->bounds_o->lower_z = pMin[2];

  args->bounds_o->upper_x = pMax[0];
  args->bounds_o->upper_y = pMax[1];
  args->bounds_o->upper_z = pMax[2];
}

void UserShapeIntersect(const RTCIntersectFunctionNArguments *args) {
  int *valid = args->valid;
  if (!valid[0])
    return;

  Shape *shape = static_cast<Shape *>(args->geometryUserPtr);
  RTCRayHit *rayhit = (RTCRayHit *)args->rayhit;

  Point3f origin{rayhit->ray.org_x, rayhit->ray.org_y, rayhit->ray.org_z};
  Vector3f direction{rayhit->ray.dir_x, rayhit->ray.dir_y, rayhit->ray.dir_z};
  Ray ray{origin, direction, 1e-4f, rayhit->ray.tfar};

  float u, v;
  int primID;
  bool hit = shape->rayIntersectShape(ray, &primID, &u, &v);
  if (hit) {
    rayhit->ray.tfar = ray.tFar;
    rayhit->hit.geomID = shape->geometryID;
    rayhit->hit.primID = primID;
    rayhit->hit.u = u;
    rayhit->hit.v = v;
  }
}

void UserShapeOcclude(const RTCOccludedFunctionNArguments *args) {
  // TODO 暂不实现
}

RTCGeometry Shape::getEmbreeGeometry(RTCDevice device) const {
  RTCGeometry geometry = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_USER);
  rtcSetGeometryUserPrimitiveCount(geometry, 1);
  rtcSetGeometryUserData(geometry, (void *)this);
  rtcSetGeometryBoundsFunction(geometry, UserShapeBound, nullptr);
  rtcSetGeometryIntersectFunction(geometry, UserShapeIntersect);
  rtcSetGeometryOccludedFunction(geometry, UserShapeOcclude);
  rtcCommitGeometry(geometry);
  return geometry;
}