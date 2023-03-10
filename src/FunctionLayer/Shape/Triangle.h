#pragma once
#include "Shape.h"
#include <FunctionLayer/Acceleration/Acceleration.h>
#include <ResourceLayer/Factory.h>
#include <ResourceLayer/Mesh.h>
class TriangleMesh;

class Triangle : public Shape {
public:
  Triangle() = default;

  Triangle(int _primID, int _vtx0Idx, int _vtx1Idx, int _vtx2Idx,
           const TriangleMesh *_mesh);

  virtual bool rayIntersectShape(Ray &ray, int *primID, float *u,
                                 float *v) const override;

  virtual void fillIntersection(float distance, int primID, float u, float v,
                                Intersection *intersection) const override;

  virtual void uniformSampleOnSurface(Vector2f sample,
                                      Intersection *intersection,
                                      float *pdf) const override {
    // TODO finish this
    return;
  }

public:
  int primID;
  int vtx0Idx, vtx1Idx, vtx2Idx;
  const TriangleMesh *mesh = nullptr;
};

class TriangleMesh : public Shape {
public:
  TriangleMesh() = default;

  TriangleMesh(const Json &json);

  //* 当使用embree时，我们使用embree内置的求交函数，故覆盖默认方法
  virtual RTCGeometry getEmbreeGeometry(RTCDevice device) const override;

  virtual bool rayIntersectShape(Ray &ray, int *primID, float *u,
                                 float *v) const override;

  virtual void fillIntersection(float distance, int primID, float u, float v,
                                Intersection *intersection) const override;

  virtual void uniformSampleOnSurface(Vector2f sample,
                                      Intersection *intersection,
                                      float *pdf) const override {
    // TODO finish this
    return;
  }

  virtual void initInternalAcceleration() override;

  friend class Triangle;

private:
  std::shared_ptr<MeshData> meshData;
  std::shared_ptr<Acceleration> acceleration;
};