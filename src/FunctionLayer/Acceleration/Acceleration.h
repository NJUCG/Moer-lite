#pragma once
#include <FunctionLayer/Ray/Ray.h>
#include <FunctionLayer/Shape/Intersection.h>
#include <FunctionLayer/Shape/Shape.h>
#include <optional>
#include <vector>

enum class AccelerationType { Embree, Linear, Octree, BVH, BVHSAH };

//* 所有空间加速结构的基类
class Acceleration {
public:
  Acceleration() = default;

  static void setAccelerationType(std::string type);

  static std::shared_ptr<Acceleration> createAcceleration();

  std::optional<Intersection> rayIntersect(Ray &ray) const {
    int primID, geomID = -1;
    float u, v;
    bool hit = rayIntersect(ray, &geomID, &primID, &u, &v);
    if (!hit)
      return std::nullopt;
    Intersection its;
    shapes[geomID]->fillIntersection(ray.tFar, primID, u, v, &its);
    return std::make_optional(its);
  }

  //* 根据场景中的所有几何体构建加速结构
  virtual void build() = 0;

  //* 向加速结构中增加一个几何体
  void attachShape(std::shared_ptr<Shape> shape) { shapes.emplace_back(shape); }

  //* 获取已添加的几何体
  std::shared_ptr<Shape> getShape(size_t index) { return shapes[index]; }

  virtual ~Acceleration() = default;
  //* 所有空间加速结构需要实现的接口
  virtual bool rayIntersect(Ray &ray, int *geomID, int *primID, float *u,
                            float *v) const = 0;

public:
  static AccelerationType type;

  AABB boundingBox;

protected:
  std::vector<std::shared_ptr<Shape>> shapes; //* 场景中的所有几何体
};

// From https://stackoverflow.com/a/75457696
// Rust-like `match` for C++
// https://en.cppreference.com/w/cpp/utility/variant/visit
template <class... Ts> struct overloaded : Ts... {
  using Ts::operator()...;
};
// explicit deduction guide (not needed as of C++20)
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

template <typename Val, typename... Ts> auto match(Val &&val, Ts... ts) {
  return std::visit(overloaded{std::forward<Ts>(ts)...},
                    std::forward<Val>(val));
}
