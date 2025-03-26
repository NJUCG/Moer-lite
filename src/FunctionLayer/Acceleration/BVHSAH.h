#pragma once
#include "Acceleration.h"
#include <span>
#include <variant>

struct BVHSAHLeaf;
struct BVHSAHInternalNode;

class BVHSAH : public Acceleration {
public:
  BVHSAH() = default;
  void build() override;
  bool rayIntersect(Ray &ray, int *geomID, int *primID, float *u,
                    float *v) const override;
  typedef std::variant<struct BVHSAHLeaf, struct BVHSAHInternalNode> BVHSAHNode;

protected:
  static constexpr int bvhLeafMaxSize = 64;
  BVHSAHNode *root = nullptr;
};

// Structs for BVH-SAH tree

struct BVHSAHLeaf {
  std::span<std::shared_ptr<Shape>> shapes;
};

struct BVHSAHInternalNode {
  AABB bounding_box;
  BVHSAH::BVHSAHNode *left = nullptr;
  BVHSAH::BVHSAHNode *right = nullptr;
  std::span<std::shared_ptr<Shape>> shapes;
  int splitAxis = -1;

  BVHSAHInternalNode(std::span<std::shared_ptr<Shape>> shapes);
};
