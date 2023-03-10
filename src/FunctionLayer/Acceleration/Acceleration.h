#pragma once
#include <FunctionLayer/Ray/Ray.h>
#include <FunctionLayer/Shape/Intersection.h>
#include <FunctionLayer/Shape/Shape.h>
#include <optional>
#include <vector>
//* 所有空间加速结构的基类
class Acceleration {
public:
  Acceleration() = default;

  virtual ~Acceleration() = default;

  //* 所有空间加速结构需要实现的接口
  virtual std::optional<Intersection> rayIntersect(Ray &ray) const = 0;

  //* 根据场景中的所有几何体构建加速结构
  virtual void build() = 0;

  //* 向加速结构中增加一个几何体
  void attachShape(std::shared_ptr<Shape> shape) {
    shape->geometryID = shapes.size();
    shapes.emplace_back(shape);
  }

protected:
  std::vector<std::shared_ptr<Shape>> shapes; //* 场景中的所有几何体
};