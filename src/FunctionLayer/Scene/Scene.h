#pragma once
#include <FunctionLayer/Acceleration/Acceleration.h>
#include <ResourceLayer/JsonUtil.h>
class Scene {
public:
  Scene() = delete;

  Scene(const Json &json);

  ~Scene() = default;

  std::optional<Intersection> rayIntersect(const Ray &ray) const;

private:
  std::shared_ptr<Acceleration> acceleration;
};
