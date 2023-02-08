#include "Parallelogram.h"
#include <ResourceLayer/Factory.h>
Parallelogram::Parallelogram(const Json &json) : Shape(json) {
  base = fetchRequired<Point3f>(json, "base");
  edge0 = fetchRequired<Vector3f>(json, "edge0");
  edge1 = fetchRequired<Vector3f>(json, "edge1");

  base = transform.toWorld(base);
  edge0 = transform.toWorld(edge0);
  edge1 = transform.toWorld(edge1);
}

//* 求几何体AABB包围盒的函数
void getParallelogramBound(const RTCBoundsFunctionArguments *args) {
  Parallelogram *paral = static_cast<Parallelogram *>(args->geometryUserPtr);
  Point3f vertices[4];
  vertices[0] = paral->base;
  vertices[1] = vertices[0] + paral->edge0;
  vertices[2] = vertices[1] + paral->edge1;
  vertices[3] = vertices[0] + paral->edge1;
  Point3f bounds[2];                   // bounds[0]是pMin, bounds[1]是pMax
  bounds[0] = bounds[1] = vertices[0]; // 初始化
  for (int dim = 0; dim < 3; ++dim) {
    for (int i = 1; i < 4; ++i) {
      bounds[0][dim] = std::min(bounds[0][dim], vertices[i][dim]);
      bounds[1][dim] = std::max(bounds[1][dim], vertices[i][dim]);
    }
  }

  args->bounds_o->lower_x = bounds[0][0];
  args->bounds_o->lower_y = bounds[0][1];
  args->bounds_o->lower_z = bounds[0][2];

  args->bounds_o->upper_x = bounds[1][0];
  args->bounds_o->upper_y = bounds[1][1];
  args->bounds_o->upper_z = bounds[1][2];

  return;
}

//* 求几何体与光线求交的函数
void rayParallelogramIntersect(const RTCIntersectFunctionNArguments *args) {
  int *valid = args->valid;
  if (!valid[0])
    return;

  Parallelogram *paral = static_cast<Parallelogram *>(args->geometryUserPtr);
  RTCRayHit *rayhit = (RTCRayHit *)args->rayhit;

  Point3f origin{rayhit->ray.org_x, rayhit->ray.org_y, rayhit->ray.org_z};
  Vector3f direction{rayhit->ray.dir_x, rayhit->ray.dir_y, rayhit->ray.dir_z};

  Vector3f paralNormal = normalize(cross(paral->edge0, paral->edge1));
  float d = -dot(paralNormal,
                 Vector3f{paral->base[0], paral->base[1], paral->base[2]});
  float a = dot(paralNormal, Vector3f{origin[0], origin[1], origin[2]}) + d;
  float b = dot(paralNormal, direction);
  if (b == .0f)
    return; // miss
  float t = -a / b;

  Point3f hitpoint = origin + t * direction;
  // hitpoint = base + u * e0 + v * e1, 0 <= u, v <= 1
  Vector3f v1 = cross(hitpoint - paral->base, paral->edge1),
           v2 = cross(paral->edge0, paral->edge1);
  float u = v1.length() / v2.length();
  if (dot(v1, v2) < 0)
    u *= -1;

  v1 = cross(hitpoint - paral->base, paral->edge0);
  v2 = cross(paral->edge1, paral->edge0);
  float v = v1.length() / v2.length();
  if (dot(v1, v2) < 0)
    v *= -1;

  if (0.f <= u && u <= 1.f && 0.f <= v && v <= 1.f) {
    rayhit->ray.tfar = t;
    rayhit->hit.Ng_x = paralNormal[0];
    rayhit->hit.Ng_y = paralNormal[1];
    rayhit->hit.Ng_z = paralNormal[2];
    rayhit->hit.geomID = paral->geometryID;
    rayhit->hit.u = u;
    rayhit->hit.v = v;
  }
}

void rayParallelogramOcclude(const RTCOccludedFunctionNArguments *args) {}

RTCGeometry Parallelogram::getEmbreeGeometry(RTCDevice device) const {
  RTCGeometry geometry = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_USER);
  rtcSetGeometryUserPrimitiveCount(geometry, 1);
  rtcSetGeometryUserData(geometry, (void *)this);
  rtcSetGeometryBoundsFunction(geometry, getParallelogramBound, nullptr);
  rtcSetGeometryIntersectFunction(geometry, rayParallelogramIntersect);
  rtcSetGeometryOccludedFunction(geometry, rayParallelogramOcclude);
  rtcCommitGeometry(geometry);
  return geometry;
}

Vector2f Parallelogram::getUVTexcod(int primID, float u, float v) const {
  return Vector2f{u, v};
}

REGISTER_CLASS(Parallelogram, "parallelogram")