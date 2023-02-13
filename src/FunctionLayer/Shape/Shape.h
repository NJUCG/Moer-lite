#pragma once
#include "Intersection.h"
#include <FunctionLayer/Material/Material.h>
#include <FunctionLayer/Ray/Ray.h>
#include <ResourceLayer/JsonUtil.h>
#include <embree3/rtcore.h>
#include <optional>

class Light;

class Shape : public Transformable {
public:
  Shape() = default;

  Shape(const Json &json);

  virtual RTCGeometry getEmbreeGeometry(RTCDevice device) const;

  // TODO 对AABB做一个封装
  std::pair<Point3f, Point3f> getAABB() const;

  virtual bool rayIntersectShape(const Ray &ray, float *distance, int *primID,
                                 float *u, float *v) const = 0;

  virtual void fillIntersection(float distance, int primID, float u, float v,
                                Intersection *intersection) const = 0;

  //* 使用一个二维随机数在shape上进行均匀随机采样
  //* 采样点的信息（位置，法线等）存储在result中，方便起见我们使用Intersection结构而不创建新的数据结构（后续可以改进）
  //* 采样结果的pdf是area measure
  virtual void uniformSampleOnSurface(Vector2f sample, Intersection *result,
                                      float *pdf) const = 0;

public:
  int geometryID;
  std::shared_ptr<Light> light;
  std::shared_ptr<Material> material;

protected:
  Point3f pMin, pMax; // AABB包围盒，构建时初始化
};