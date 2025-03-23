#pragma once
#include "Acceleration.h"
#include <span>
#include <variant>

struct BVHLeaf;
struct BVHInternalNode;

class BVH : public Acceleration {
public:
  BVH() = default;
  void build() override;
  bool rayIntersect(Ray &ray, int *geomID, int *primID, float *u,
                    float *v) const override;
  typedef std::variant<struct BVHLeaf, struct BVHInternalNode> BVHNode;

protected:
  static constexpr int bvhLeafMaxSize = 64;
  BVHNode *root = nullptr;
};

// Structs for BVH tree

struct BVHLeaf {
  std::span<std::shared_ptr<Shape>> shapes;
};

struct BVHInternalNode {
  AABB bounding_box;
  BVH::BVHNode *left = nullptr;
  BVH::BVHNode *right = nullptr;
  std::span<std::shared_ptr<Shape>> shapes;

  BVHInternalNode(std::span<std::shared_ptr<Shape>> shapes);
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
