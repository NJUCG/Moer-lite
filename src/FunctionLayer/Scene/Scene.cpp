#include "Scene.h"
#include <FunctionLayer/Acceleration/EmbreeBVH.h>
#include <FunctionLayer/Acceleration/Linear.h>
#include <FunctionLayer/Light/AreaLight.h>
#include <ResourceLayer/Factory.h>

Scene::Scene(const Json &json) {
  //* 初始化加速结构，默认使用embree
  std::string accelerationType =
      fetchOptional<std::string>(json, "acceleration", "embree");
  Acceleration::setAccelerationType(accelerationType);
  acceleration = Acceleration::createAcceleration();

  //* 添加几何体
  int geomID = 0;
  auto shapes = json["shapes"];
  for (int i = 0; i < shapes.size(); ++i) {
    auto shape = Factory::construct_class<Shape>(shapes[i]);
    shape->geometryID = geomID++;
    acceleration->attachShape(shape);
  }
  //* 添加光源
  auto lights = json["lights"];
  std::vector<std::shared_ptr<Light>> lightsVec;
  for (int i = 0; i < lights.size(); ++i) {
    auto light = Factory::construct_class<Light>(lights[i]);
    //* 如果是环境光源，环境光源不加入光源分布
    if (light->type == LightType::EnvironmentLight) {
      this->infiniteLights.emplace_back(
          std::static_pointer_cast<InfiniteLight>(light));
      continue;
    }
    lightsVec.emplace_back(light);
    //* 如果是面光源，将其shape也加入加速结构
    if (light->type == LightType::AreaLight) {
      auto shape = std::static_pointer_cast<AreaLight>(light)->shape;
      shape->light = light;
      shape->geometryID = geomID++;
      acceleration->attachShape(shape);
    }
  }
  //* 产生一个均匀光源分布，每个光源被采样到的几率是一样的
  lightDistribution = Distribution<std::shared_ptr<Light>>(
      lightsVec, [](std::shared_ptr<Light> light) -> float { return 1.f; });

  //* 构建加速结构
  acceleration->build();
}

std::optional<Intersection> Scene::rayIntersect(Ray &ray) const {
  return acceleration->rayIntersect(ray);
}

std::shared_ptr<Light> Scene::sampleLight(float sample, float *pdf) const {
  return lightDistribution.sample(sample, pdf);
}