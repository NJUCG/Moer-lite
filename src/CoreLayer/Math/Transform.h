#pragma once
#include "Geometry.h"
#include "FunctionLayer/Acceleration/AABB.h"
//* 三维空间中的旋转、缩放、平移操作均可以用一个4x4的矩阵进行描述，Transform对象描述了如何对一个物体在三维空间上进行变换
//* Transform是对4维方阵的高层次抽象，只关心旋转、缩放、平移操作
struct Transform {
public:
  //* 返回一个三维空间平移操作的矩阵描述，offset是平移的偏移量
  static Matrix4f translation(const Vector3f &offset);
  //* 返回一个三维空间旋转操作的矩阵描述，axis是旋转轴，radian是旋转的弧度
  static Matrix4f rotation(const Vector3f &axis, float radian);
  //* 返回一个三维空间缩放操作的矩阵描述，scale是沿xyz轴的缩放比例
  static Matrix4f scalation(const Vector3f &scale);

  //* 默认构造函数，所有矩阵都是4x4的单位矩阵
  Transform();

  Transform(const Matrix4f &_translation, const Matrix4f &_rotation,
            const Matrix4f &_scalation);

  //* vector是坐标系C中的一个局部向量（即vector在C中不发生相对变换）
  //* 当对C应用该Transform后，toWorld返回vector发生变换后的世界坐标系表达
  Vector3f toWorld(const Vector3f &vector) const;

  //* point是坐标系C中的一个局部点（即point在C中不发生相对变换）
  //* 当对C应用该Transform后，toWorld返回point发生变换后的世界坐标系表达
  Point3f toWorld(const Point3f &point) const;

  //*
  //* 对包围盒进行变换
  AABB toWorld(const AABB & box) const;

  Ray inverseRay(const Ray & ray) const;

public:
  Matrix4f translate, invTranslate;
  Matrix4f rotate, invRotate;
  Matrix4f scale, invScale;
};

//* 所有在3维空间中可进行旋转、缩放、平移操作的对象都需要继承该类，并实现所有接口
class Transformable {
public:
  Transformable() = default;

  Transformable(const Transform &_transform) : transform(_transform) {}

  //* 对该对象应用一个transform变换
  // TODO
  virtual void apply(const Transform &_transform){};

protected:
  Transform transform; // 该对象当前的transform
};
