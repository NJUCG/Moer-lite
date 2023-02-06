#include "Scene.h"
#include <FunctionLayer/Acceleration/EmbreeBVH.h>
#include <ResourceLayer/Factory.h>

Scene::Scene(const Json &json) {
  //* 初始化加速结构
  acceleration = std::make_shared<EmbreeBVH>();

  //* 添加几何体
  auto shapes = json["shapes"];
  for (int i = 0; i < shapes.size(); ++i) {
    const auto &shapeType = fetchRequired<std::string>(shapes[i], "type");
    auto shape = Factory::construct_class<Shape>(shapeType, shapes[i]);
    acceleration->attachShape(shape);
  }

  //* 构建加速结构
  acceleration->build();
}

std::optional<Intersection> Scene::rayIntersect(const Ray &ray) const {
  return acceleration->rayIntersect(ray);
}
