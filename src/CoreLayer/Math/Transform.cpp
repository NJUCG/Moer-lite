#include "Transform.h"
using namespace vecmat;

Transform::Transform() {
  translate = invTranslate = rotate = invRotate = scale = invScale =
      mat44f::identity();
}

Transform Transform::translation(const Vector3f &offset) {
  Transform transform;
  for (int i = 0; i < 3; ++i) {
    transform.translate.rows[i][3] = offset[i];
    transform.invTranslate.rows[i][3] = -offset[i];
  }
  return transform;
}

Transform Transform::rotation(const Vector3f &axis, float radian) {
  Transform transform;

  Vector3f a = normalize(axis);
  float sinTheta = fm::sin(radian), cosTheta = fm::cos(radian);
  transform.rotate.rows[0][0] = a[0] * a[0] + (1 - a[0] * a[0]) * cosTheta;
  transform.rotate.rows[0][1] = a[0] * a[1] * (1 - cosTheta) - a[2] * sinTheta;
  transform.rotate.rows[0][2] = a[0] * a[2] * (1 - cosTheta) + a[1] * sinTheta;
  transform.rotate.rows[0][3] = 0;

  transform.rotate.rows[1][0] = a[0] * a[1] * (1 - cosTheta) + a[2] * sinTheta;
  transform.rotate.rows[1][1] = a[1] * a[1] + (1 - a[1] * a[1]) * cosTheta;
  transform.rotate.rows[1][2] = a[1] * a[2] * (1 - cosTheta) - a[0] * sinTheta;
  transform.rotate.rows[1][3] = 0;

  transform.rotate.rows[2][0] = a[0] * a[2] * (1 - cosTheta) - a[1] * sinTheta;
  transform.rotate.rows[2][1] = a[1] * a[2] * (1 - cosTheta) + a[0] * sinTheta;
  transform.rotate.rows[2][2] = a[2] * a[2] + (1 - a[2] * a[2]) * cosTheta;
  transform.rotate.rows[2][3] = 0;

  transform.rotate.rows[3][0] = 0;
  transform.rotate.rows[3][1] = 0;
  transform.rotate.rows[3][2] = 0;
  transform.rotate.rows[3][3] = 1;

  transform.invRotate = transform.rotate.transpose();
  return transform;
}

Transform Transform::scalation(const Vector3f &scale) {
  Transform transform;
  for (int i = 0; i < 3; ++i) {
    transform.scale.rows[i][i] = scale[i];
    transform.invScale.rows[i][i] = 1.f / scale[i];
  }
  return transform;
}