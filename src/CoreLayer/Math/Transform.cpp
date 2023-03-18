#include "Transform.h"

Matrix4f Transform::translation(const Vector3f &offset) {
  Matrix4f mat = Matrix4f::identity();
  for (int i = 0; i < 3; ++i) {
    mat.rows[i][3] = offset[i];
  }
  return mat;
}

Matrix4f Transform::rotation(const Vector3f &axis, float radian) {
  Vector3f a = normalize(axis);
  float sin_theta = std::sin(radian), cos_theta = std::cos(radian);
  Matrix4f mat;

  mat.rows[0][0] = a[0] * a[0] + (1 - a[0] * a[0]) * cos_theta;
  mat.rows[0][1] = a[0] * a[1] * (1 - cos_theta) - a[2] * sin_theta;
  mat.rows[0][2] = a[0] * a[2] * (1 - cos_theta) + a[1] * sin_theta;
  mat.rows[0][3] = 0;

  mat.rows[1][0] = a[0] * a[1] * (1 - cos_theta) + a[2] * sin_theta;
  mat.rows[1][1] = a[1] * a[1] + (1 - a[1] * a[1]) * cos_theta;
  mat.rows[1][2] = a[1] * a[2] * (1 - cos_theta) - a[0] * sin_theta;
  mat.rows[1][3] = 0;

  mat.rows[2][0] = a[0] * a[2] * (1 - cos_theta) - a[1] * sin_theta;
  mat.rows[2][1] = a[1] * a[2] * (1 - cos_theta) + a[0] * sin_theta;
  mat.rows[2][2] = a[2] * a[2] + (1 - a[2] * a[2]) * cos_theta;
  mat.rows[2][3] = 0;

  mat.rows[3][0] = 0;
  mat.rows[3][1] = 0;
  mat.rows[3][2] = 0;
  mat.rows[3][3] = 1;

  return mat;
}

Matrix4f Transform::scalation(const Vector3f &scale) {
  Matrix4f mat = Matrix4f::identity();
  for (int i = 0; i < 3; ++i) {
    mat.rows[i][i] = scale[i];
  }
  return mat;
}

Transform::Transform() {
  translate = invTranslate = rotate = invRotate = scale = invScale =
      Matrix4f::identity();
}

Transform::Transform(const Matrix4f &_translation, const Matrix4f &_rotation,
                     const Matrix4f &_scalation)
    : translate(_translation), rotate(_rotation), scale(_scalation) {
  invTranslate = invRotate = invScale = Matrix4f::identity();
  for (int i = 0; i < 3; ++i) {
    invTranslate.rows[i][3] = -translate.rows[i][3];
    invScale.rows[i][i] = 1.f / scale.rows[i][i];
  }
  invRotate = rotate.transpose();
}

Vector3f Transform::toWorld(const Vector3f &vector) const {
  vecmat::vec4f v4{vector[0], vector[1], vector[2], 0};
  v4 = translate * rotate * scale * v4;
  return Vector3f{v4[0], v4[1], v4[2]};
}

Point3f Transform::toWorld(const Point3f &point) const {
  vecmat::vec4f v4{point[0], point[1], point[2], 1};
  v4 = translate * rotate * scale * v4;
  v4 /= v4[3];
  return Point3f{v4[0], v4[1], v4[2]};
}

AABB Transform::toWorld(const AABB &b) const {
    AABB ret;
    ret = ret.Union(toWorld(Point3f(b.pMin[0], b.pMin[1], b.pMin[2])));
    ret = ret.Union(toWorld(Point3f(b.pMax[0], b.pMin[1], b.pMin[2])));
    ret = ret.Union(toWorld(Point3f(b.pMin[0], b.pMax[1], b.pMin[2])));
    ret = ret.Union(toWorld(Point3f(b.pMin[0], b.pMin[1], b.pMax[2])));
    ret = ret.Union(toWorld(Point3f(b.pMin[0], b.pMax[1], b.pMax[2])));
    ret = ret.Union(toWorld(Point3f(b.pMax[0], b.pMax[1], b.pMin[2])));
    ret = ret.Union(toWorld(Point3f(b.pMax[0], b.pMin[1], b.pMax[2])));
    ret = ret.Union(toWorld(Point3f(b.pMax[0], b.pMax[1], b.pMax[2])));
    return ret;
}

Ray Transform::inverseRay(const Ray &ray) const {
    Point3f origin = ray.origin;
    Vector3f direction = ray.direction;

    vecmat::vec4f o{origin[0], origin[1], origin[2], 1.f},
            d{direction[0], direction[1], direction[2], 0.f};

    o = invRotate * invTranslate * o;
    d = invRotate * invTranslate * d;

    o /= o[3];
    origin = Point3f{o[0], o[1], o[2]};
    direction = Vector3f{d[0], d[1], d[2]};
    return {origin,direction,ray.tNear,ray.tFar};
}
