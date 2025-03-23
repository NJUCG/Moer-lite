#include "AABB.h"
#include "CoreLayer/Math/Geometry.h"

Point3f minP(const Point3f &p1, const Point3f &p2) {
  return Point3f{std::min(p1[0], p2[0]), std::min(p1[1], p2[1]),
                 std::min(p1[2], p2[2])};
}

Point3f maxP(const Point3f &p1, const Point3f &p2) {
  return Point3f{std::max(p1[0], p2[0]), std::max(p1[1], p2[1]),
                 std::max(p1[2], p2[2])};
}

AABB AABB::Union(const AABB &other) const {
  Point3f min = minP(other.pMin, pMin), max = maxP(other.pMax, pMax);
  return AABB{min, max};
}

void AABB::Expand(const AABB &other) {
  pMin = minP(pMin, other.pMin);
  pMax = maxP(pMax, other.pMax);
}

AABB AABB::Union(const Point3f &other) const {
  Point3f min = minP(other, pMin), max = maxP(other, pMax);
  return AABB{min, max};
}

void AABB::Expand(const Point3f &other) {
  pMin = minP(pMin, other);
  pMax = maxP(pMax, other);
}

bool AABB::Overlap(const AABB &other) const {
  for (int dim = 0; dim < 3; ++dim) {
    if (pMin[dim] > other.pMax[dim] || pMax[dim] < other.pMin[dim]) {
      return false;
    }
  }
  return true;
}

Vector2f const2vec(const float x) { return Vector2f(x, x); }

Vector3f vec_min(const Vector3f &v1, const Vector3f &v2) {
  return Vector3f{std::min(v1[0], v2[0]), std::min(v1[1], v2[1]),
                  std::min(v1[2], v2[2])};
}

Vector3f vec_max(const Vector3f &v1, const Vector3f &v2) {
  return Vector3f{std::max(v1[0], v2[0]), std::max(v1[1], v2[1]),
                  std::max(v1[2], v2[2])};
}

Vector3f clamp(const Vector3f &v, float min_val, float max_val) {
  return Vector3f{std::clamp(v[0], min_val, max_val),
                  std::clamp(v[1], min_val, max_val),
                  std::clamp(v[2], min_val, max_val)};
}

bool AABB::RayIntersect(const Ray &ray, float *tMin, float *tMax) const {
  auto tA = (pMin - ray.origin) / ray.direction;
  auto tB = (pMax - ray.origin) / ray.direction;

  auto t_near = clamp(vec_min(tA, tB), 0, ray.tFar);
  auto t_far = clamp(vec_max(tA, tB), 0, ray.tFar);

  auto latest_entry = std::max({t_near[0], t_near[1], t_near[2]});
  auto earliest_exit = std::min({t_far[0], t_far[1], t_far[2]});

  auto result = latest_entry <= earliest_exit;

  // if (result) {
  //   ray.tFar = earliest_exit;
  // }

  return result;
}

Point3f AABB::Center() const {
  return Point3f{(pMin[0] + pMax[0]) * .5f, (pMin[1] + pMax[1]) * .5f,
                 (pMin[2] + pMax[2]) * .5f};
}
