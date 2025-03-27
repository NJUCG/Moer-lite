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
  int splitAxis;

  BVHInternalNode(std::span<std::shared_ptr<Shape>> shapes);
};
