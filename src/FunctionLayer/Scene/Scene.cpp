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

  //* 添加光源
  auto lights = json["lights"];
  std::vector<std::shared_ptr<Light>> lightsVec;
  for (int i = 0; i < lights.size(); ++i) {
    const auto &lightType = fetchRequired<std::string>(lights[i], "type");
    auto light = Factory::construct_class<Light>(lightType, lights[i]);
    lightsVec.emplace_back(light);
  }
  lightDistribution = Distribution<std::shared_ptr<Light>>(
      lightsVec, [](std::shared_ptr<Light> light) -> float { return 1.f; });

  //* 构建加速结构
  acceleration->build();
}

std::optional<Intersection> Scene::rayIntersect(const Ray &ray) const {
  return acceleration->rayIntersect(ray);
}

std::shared_ptr<Light> Scene::sampleLight(float sample, float *pdf) const {
  return lightDistribution.sample(sample, pdf);
}