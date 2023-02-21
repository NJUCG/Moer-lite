#include "Pinhole.h"

PinholeCamera::PinholeCamera(const Json &json) : PerspectiveCamera(json) {}

Ray PinholeCamera::sampleRay(const CameraSample &sample, Vector2f NDC) const {
  float x = (NDC[0] - 0.5f) * film->size[0] + sample.xy[0],
        y = (0.5f - NDC[1]) * film->size[1] + sample.xy[1];

  float tanHalfFov = fm::tan(verticalFov * 0.5f);
  float z = -film->size[1] * 0.5f / tanHalfFov;

  Vector3f direction = Vector3f{x, y, z};
  direction = normalize(transform.toWorld(direction));
  Point3f origin = transform.toWorld(Point3f(0));

  // TODO 如果要实现动态模糊，那么此处需要根据sample中的time采样一个时间
  return Ray(origin, direction, tNear, tFar, timeStart);
}

Ray PinholeCamera::sampleRayDifferentials(const CameraSample &sample,
                                          Vector2f NDC) const {
  float x = (NDC[0] - 0.5f) * film->size[0] + sample.xy[0],
        y = (0.5f - NDC[1]) * film->size[1] + sample.xy[1];

  float tanHalfFov = fm::tan(verticalFov * 0.5f);
  float z = -film->size[1] * 0.5f / tanHalfFov;

  Vector3f direction = normalize(transform.toWorld(Vector3f{x, y, z})),
           directionX = normalize(transform.toWorld(Vector3f{x + 1.f, y, z})),
           directionY = normalize(transform.toWorld(Vector3f{x, y + 1.f, z}));
  Point3f origin = transform.toWorld(Point3f(0));

  // TODO 如果要实现动态模糊，那么此处需要根据sample中的time采样一个时间
  Ray ret = Ray(origin, direction, tNear, tFar, timeStart);

  ret.hasDifferentials = true;
  ret.directionX = directionX;
  ret.directionY = directionY;
  ret.originX = ret.originY = origin;
  return ret;
}

REGISTER_CLASS(PinholeCamera, "pinhole")