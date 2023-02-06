#include "Scene.h"
#include <FunctionLayer/Acceleration/EmbreeBVH.h>

Scene::Scene(const Json &json) {
  //* 初始化加速结构
  acceleration = std::make_shared<EmbreeBVH>();

  //* 添加几何体

  //* 构建加速结构
  acceleration->build();
}

std::optional<Intersection> Scene::rayIntersect(const Ray &ray) const {
  return acceleration->rayIntersect(ray);
}
