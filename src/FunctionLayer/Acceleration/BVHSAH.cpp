#include "BVHSAH.h"
#include "FunctionLayer/Acceleration/AABB.h"
#include <algorithm>
#include <cassert>
#include <cstddef>

// Create a BVH node from a span of shapes
//
// Creates a leaf node if the span contains less than 5 shapes,
// otherwise creates an internal node
BVHSAH::BVHSAHNode *
create_bvh_sah_node(std::span<std::shared_ptr<Shape>> shapes) {
  assert(shapes.size() > 0);
  if (shapes.size() <= 10)
    return new BVHSAH::BVHSAHNode(BVHSAHLeaf(shapes));
  else
    return new BVHSAH::BVHSAHNode(BVHSAHInternalNode(shapes));
}

void BVHSAH::build() {
  AABB sceneBox;
  for (const auto &shape : shapes) {
    //* 自行实现的加速结构请务必对每个shape调用该方法，以保证TriangleMesh构建内部加速结构
    //* 由于使用embree时，TriangleMesh::getAABB不会被调用，因此出于性能考虑我们不在TriangleMesh
    //* 的构造阶段计算其AABB，因此当我们将TriangleMesh的AABB计算放在TriangleMesh::initInternalAcceleration中
    //* 所以请确保在调用TriangleMesh::getAABB之前先调用TriangleMesh::initInternalAcceleration
    shape->initInternalAcceleration();
    sceneBox.Expand(shape->getAABB());
  }

  root = create_bvh_sah_node(shapes);
}

// Calculates the cost of splitting the shapes at the given index.
//
// shapes[index] would be the second part
//
// Split axis isn't set here; sort `shapes` according to that axis
// before calling this function.
float costAtSplit(std::span<std::shared_ptr<Shape>> shapes, int splitIndex) {
  auto C_t = 1.0f, C_i = 1.0f;

  auto left_box = shapes[0]->getAABB();
  for (auto shape : shapes.first(splitIndex)) {
    left_box.Expand(shape->getAABB());
  }

  auto right_box = shapes[splitIndex]->getAABB();
  for (auto shape : shapes.subspan(splitIndex)) {
    right_box.Expand(shape->getAABB());
  }

  auto all_box = left_box.Union(right_box);

  auto left_cost =
      left_box.SurfaceArea() / all_box.SurfaceArea() * splitIndex * C_i;
  auto right_cost = right_box.SurfaceArea() / all_box.SurfaceArea() *
                    (shapes.size() - splitIndex) * C_i;
  return C_t + left_cost + right_cost;
}

// For shapes sorted on a whatever axis, get the best cost and split index
std::tuple<float, size_t> getBestSplit(std::span<std::shared_ptr<Shape>> shapes,
                                       size_t samplePoints = 100) {
  assert(shapes.size() >= 2);
  samplePoints = std::min(samplePoints, shapes.size() - 1);

  size_t minIndex = 1, maxIndex = shapes.size() - 1,
         distance = maxIndex - minIndex;

  float bestCost = std::numeric_limits<float>::max();
  size_t bestIndex = 0;
  for (auto i = 0; i < samplePoints; i++) {
    float progress = float(i) / (samplePoints - 1);
    size_t splitIndex = minIndex + progress * distance;
    auto cost = costAtSplit(shapes, splitIndex);

    if (cost < bestCost) {
      bestCost = cost;
      bestIndex = splitIndex;
    }
  }

  return std::make_tuple(bestCost, bestIndex);
}

BVHSAHInternalNode::BVHSAHInternalNode(
    std::span<std::shared_ptr<Shape>> shapes) {

  this->shapes = shapes;
  this->bounding_box = AABB();
  for (auto shape : shapes) {
    this->bounding_box.Expand(shape->getAABB());
  }

  float bestCost = std::numeric_limits<float>::max();
  size_t bestIndex = 0;
  int bestAxis = -1;
  for (auto axis : {0, 1, 2}) {
    std::sort(shapes.begin(), shapes.end(), [&](auto a, auto b) {
      return a->getAABB().Center()[axis] < b->getAABB().Center()[axis];
    });

    auto [cost, splitIndex] = getBestSplit(shapes);
    if (cost < bestCost) {
      bestCost = cost;
      bestIndex = splitIndex;
      bestAxis = axis;
    }
  }

  splitAxis = bestAxis;
  std::sort(shapes.begin(), shapes.end(), [&](auto a, auto b) {
    return a->getAABB().Center()[bestAxis] < b->getAABB().Center()[bestAxis];
  });

#pragma omp parallel sections
  {
#pragma omp section
    {
      left = create_bvh_sah_node(shapes.first(bestIndex));
    }
#pragma omp section
    {
      right = create_bvh_sah_node(shapes.subspan(bestIndex));
    }
  }

  assert(left != nullptr && right != nullptr);
}

bool BVHSAHNode_rayIntersect(BVHSAH::BVHSAHNode *root, Ray &ray, int *geomID,
                             int *primID, float *u, float *v, int depth) {
  return match(
      *root,
      [&](BVHSAHLeaf leaf) {
        for (auto shape : leaf.shapes) {
          if (shape->rayIntersectShape(ray, primID, u, v)) {
            *geomID = shape->geometryID;
            return true;
          }
        }
        return false;
      },
      [&](BVHSAHInternalNode node) {
        assert(node.splitAxis != -1);
        if (node.bounding_box.RayIntersect(ray) == false)
          return false;

        bool intersect_left = false, intersect_right = false;
        if (ray.direction[node.splitAxis] >= 0) {
          // Positive on splitted axis
          // Will check left child first
          intersect_left = BVHSAHNode_rayIntersect(node.left, ray, geomID,
                                                   primID, u, v, depth + 1);
          intersect_right = BVHSAHNode_rayIntersect(node.right, ray, geomID,
                                                    primID, u, v, depth + 1);
        } else {
          // Negative on splitted axis
          // Will check right child first
          intersect_right = BVHSAHNode_rayIntersect(node.right, ray, geomID,
                                                    primID, u, v, depth + 1);
          intersect_left = BVHSAHNode_rayIntersect(node.left, ray, geomID,
                                                   primID, u, v, depth + 1);
        }
        return intersect_left || intersect_right;
      });
}

bool BVHSAH::rayIntersect(Ray &ray, int *geomID, int *primID, float *u,
                          float *v) const {
  return BVHSAHNode_rayIntersect(root, ray, geomID, primID, u, v, 0);
}
