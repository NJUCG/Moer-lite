#include "Sphere.h"
#include <ResourceLayer/Factory.h>

Sphere::Sphere(const Json &json) : Shape(json) {
  center = fetchRequired<Point3f>(json, "center");
  radius = fetchRequired<float>(json, "radius");

  center = transform.toWorld(center);
}

//* 求几何体AABB包围盒的函数
void getSphereBound(const RTCBoundsFunctionArguments *args) {
  Sphere *sphere = static_cast<Sphere *>(args->geometryUserPtr);
  Point3f center = sphere->center;
  float radius = sphere->radius;
  Point3f pMin = center - Vector3f(radius), pMax = center + Vector3f(radius);

  args->bounds_o->lower_x = pMin[0];
  args->bounds_o->lower_y = pMin[1];
  args->bounds_o->lower_z = pMin[2];

  args->bounds_o->upper_x = pMax[0];
  args->bounds_o->upper_y = pMax[1];
  args->bounds_o->upper_z = pMax[2];
}

//* 求几何体与光线求交的函数
void raySphereIntersect(const RTCIntersectFunctionNArguments *args) {
  int *valid = args->valid;
  if (!valid[0])
    return;

  Sphere *sphere = static_cast<Sphere *>(args->geometryUserPtr);
  RTCRayHit *rayhit = (RTCRayHit *)args->rayhit;

  Point3f origin{rayhit->ray.org_x, rayhit->ray.org_y, rayhit->ray.org_z};
  Vector3f direction{rayhit->ray.dir_x, rayhit->ray.dir_y, rayhit->ray.dir_z};

  Vector3f u = sphere->center - origin;
  float b = dot(u, direction);
  float c = u.length() * u.length() - sphere->radius * sphere->radius;
  float delta = b * b - c;
  if (delta <= 0)
    return; // 不相交
  float sqrtDelta = fm::sqrt(delta);
  float t1 = b - sqrtDelta;
  float t2 = b + sqrtDelta;

  if (rayhit->ray.tnear <= t2 && t2 <= rayhit->ray.tfar) {
    rayhit->ray.tfar = t2;
    Vector3f normal = normalize(origin + t2 * direction - sphere->center);
    rayhit->hit.Ng_x = normal[0];
    rayhit->hit.Ng_y = normal[1];
    rayhit->hit.Ng_z = normal[2];
    rayhit->hit.geomID = sphere->geometryID;
  }
  if (rayhit->ray.tnear <= t1 && t1 <= rayhit->ray.tfar) {
    rayhit->ray.tfar = t1;
    Vector3f normal = normalize(origin + t1 * direction - sphere->center);
    rayhit->hit.Ng_x = normal[0];
    rayhit->hit.Ng_y = normal[1];
    rayhit->hit.Ng_z = normal[2];
    rayhit->hit.geomID = sphere->geometryID;
  }
}

// TODO
void raySphereOcclude(const RTCOccludedFunctionNArguments *args) {}

RTCGeometry Sphere::getEmbreeGeometry(RTCDevice device) const {
  RTCGeometry geometry = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_USER);
  rtcSetGeometryUserPrimitiveCount(geometry, 1);
  rtcSetGeometryUserData(geometry, (void *)this);
  rtcSetGeometryBoundsFunction(geometry, getSphereBound, nullptr);
  rtcSetGeometryIntersectFunction(geometry, raySphereIntersect);
  rtcSetGeometryOccludedFunction(geometry, raySphereOcclude);
  rtcCommitGeometry(geometry);
  return geometry;
}

REGISTER_CLASS(Sphere, "sphere")