#pragma once
#include <CoreLayer/Math/Math.h>

struct Ray {
  Ray() = default;

  Ray(Point3f _origin, Vector3f _direction, float _tNear = 1e-4f,
      float _tFar = 1e10f, float _time = .0f)
      : origin(_origin), direction(normalize(_direction)), tNear(_tNear),
        tFar(_tFar), time(_time) {}
  Ray(Point3f _origin, Point3f _destination, float _time = .0f)
      : origin(_origin), tNear(1e-4f), time(_time) {
    Vector3f o2d = _destination - _origin;
    tFar = o2d.length() - 1e-4;
    direction = normalize(o2d);
  }

  Point3f origin;
  Vector3f direction;
  float tFar, tNear, time;
};