#include "Scene.h"
#include <FunctionLayer/Acceleration/EmbreeBVH.h>
#include <FunctionLayer/Light/AreaLight.h>
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
    //* 如果是面光源，将其shape也加入加速结构
    if (light->type == LightType::AreaLight) {
      acceleration->attachShape(
          std::static_pointer_cast<AreaLight>(light)->shape);
    }
  }
  //* 产生一个均匀光源分布，每个光源被采样到的几率是一样的
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