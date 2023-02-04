#pragma once
#include "Geometry.h"

//* 三维空间中的旋转、缩放、平移操作均可以用一个4x4的矩阵进行描述，Transform对象描述了如何对一个物体在三维空间上进行变换
//* Transform是对4维方阵的高层次抽象，只关心旋转、缩放、平移操作
struct Transform {
public:
  Transform();
  static Transform translation(const Vector3f &offset);
  static Transform rotation(const Vector3f &axis, float radian);
  static Transform scalation(const Vector3f &scale);

protected:
  vecmat::mat44f translate, invTranslate;
  vecmat::mat44f rotate, invRotate;
  vecmat::mat44f scale, invScale;
};

//* 所有在3维空间中可进行旋转、缩放、平移操作的对象都需要继承该类，并实现所有接口
class Transformable {
public:
  Transformable() = default;

  //* 对该对象应用一个transform变换
  virtual void apply(const Transform &_transform) = 0;

protected:
  Transform transform; // 该对象当前的transform
};
