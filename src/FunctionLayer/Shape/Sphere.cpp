#include "Sphere.h"
#include <ResourceLayer/Factory.h>

Sphere::Sphere(const Json &json) : Shape(json) {
  center = fetchRequired<Point3f>(json, "center");
  radius = fetchRequired<float>(json, "radius");

  center = transform.toWorld(center);
  boundingBox = AABB(center - Vector3f(radius), center + Vector3f(radius));
}

bool Sphere::rayIntersectShape(Ray &ray, int *primID, float *u,
                               float *v) const {
  Point3f origin = ray.origin;
  Vector3f direction = ray.direction;
  Vector3f o2c = center - origin;
  float b = dot(o2c, direction);
  float c = o2c.length() * o2c.length() - radius * radius;
  float delta = b * b - c;
  if (delta <= 0)
    return false; // 不相交
  float sqrtDelta = fm::sqrt(delta);
  float t1 = b - sqrtDelta;
  float t2 = b + sqrtDelta;

  bool hit = false;
  if (ray.tNear <= t2 && t2 <= ray.tFar) {
    ray.tFar = t2;
    hit = true;
  }
  if (ray.tNear <= t1 && t1 <= ray.tFar) {
    ray.tFar = t1;
    hit = true;
  }
  if (hit) {
    *primID = 0;
    //* 计算u,v
    // TODO 需要考虑旋转
    Vector3f normal = normalize(ray.at(ray.tFar) - center);
    float cosTheta = normal[1];
    *v = fm::acos(cosTheta);
    if (std::abs(normal[2]) < 1e-4f) {
      *u = (normal[0] > .0f) ? (PI * .5f) : (PI * 1.5f);
    } else {
      float tanPhi = normal[0] / normal[2];
      *u = fm::atan(tanPhi); // u in [-.5f * PI, .5f * PI]
      if (normal[2] < .0f)
        *u += PI;
    }
  }
  return hit;
}

void Sphere::fillIntersection(float distance, int primID, float u, float v,
                              Intersection *intersection) const {
  // u->phi, v->theta

  intersection->shape = this;
  intersection->distance = distance;
  Vector3f normal = Vector3f{std::sin(v) * std::sin(u), std::cos(v),
                             std::sin(v) * std::cos(u)};

  intersection->normal = normal;

  //* 计算交点
  Point3f position = center + radius * normal;
  intersection->position = position;

  //* 计算纹理坐标
  intersection->texCoord = Vector2f{u * INV_PI * .5f, v * INV_PI};

  // TODO 计算交点的切线和副切线
  Vector3f tangent{1.f, 0.f, .0f};
  Vector3f bitangent;
  if (std::abs(dot(tangent, intersection->normal)) > .9f) {
    tangent = Vector3f(.0f, 1.f, .0f);
  }
  bitangent = normalize(cross(tangent, intersection->normal));
  tangent = normalize(cross(intersection->normal, bitangent));
  intersection->tangent = tangent;
  intersection->bitangent = bitangent;
}

REGISTER_CLASS(Sphere, "sphere")