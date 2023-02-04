#include "Pinhole.h"

PinholeCamera::PinholeCamera(const Json &json) : PerspectiveCamera(json) {}

Ray PinholeCamera::sampleRay(const CameraSample &sample, Vector2f NDC) const {
  float x = (NDC[0] - 0.5f) * film->size[0] + sample.xy[0],
        y = (NDC[1] - 0.5f) * film->size[1] + sample.xy[1];

  x /= film->size[0] * 0.5f;
  y /= film->size[1] * 0.5f;

  float halfH = fm::tan(verticalFov * 0.5f);
  x *= aspectRatio * halfH;
  y *= -halfH;

  Vector3f direction = Point3f{x, y, -1} - Point3f(0);
  direction = transform.toWorld(direction);
  Point3f origin = transform.toWorld(Point3f(0));

  // TODO 如果要实现动态模糊，那么此处需要根据sample中的time采样一个时间
  return Ray(origin, direction, tNear, tFar, timeStart);
}

REGISTER_CLASS(PinholeCamera, "pinhole")