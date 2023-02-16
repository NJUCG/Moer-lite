#pragma once
#include <CoreLayer/Math/Math.h>

class Shape;

//* Ray与Shape交点处的信息
struct Intersection {
  float distance;              // 从光线起点到交点的距离
  Point3f position;            // 交点的位置
  Vector3f normal;             // 交点处的法线
  Vector3f tangent, bitangent; // 交点处的切线和副切线
  Vector2f texCoord;           // 交点处的纹理坐标
  const Shape *shape;          // 指向与光线相交的物体
  Vector3f dpdu, dpdv;         // 交点处沿u、v方向的切线
};