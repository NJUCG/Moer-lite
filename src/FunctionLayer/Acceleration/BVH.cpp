#include "BVH.h"
#include "FunctionLayer/Acceleration/AABB.h"
#include <algorithm>
#include <cassert>

// Create a BVH node from a span of shapes
//
// Creates a leaf node if the span contains less than 5 shapes,
// otherwise creates an internal node
BVH::BVHNode *create_bvh_node(std::span<std::shared_ptr<Shape>> shapes) {
  assert(shapes.size() > 0);
  if (shapes.size() < 5)
    return new BVH::BVHNode(BVHLeaf(shapes));
  else
    return new BVH::BVHNode(BVHInternalNode(shapes));
}

void BVH::build() {
  AABB sceneBox;
  for (const auto &shape : shapes) {
    //* 自行实现的加速结构请务必对每个shape调用该方法，以保证TriangleMesh构建内部加速结构
    //* 由于使用embree时，TriangleMesh::getAABB不会被调用，因此出于性能考虑我们不在TriangleMesh
    //* 的构造阶段计算其AABB，因此当我们将TriangleMesh的AABB计算放在TriangleMesh::initInternalAcceleration中
    //* 所以请确保在调用TriangleMesh::getAABB之前先调用TriangleMesh::initInternalAcceleration
    shape->initInternalAcceleration();
    sceneBox.Expand(shape->getAABB());
  }

  // Sort all shapes according to their center x-coordinate
  std::sort(shapes.begin(), shapes.end(), [](auto a, auto b) {
    auto centerA = a->getAABB().Center();
    auto centerB = b->getAABB().Center();
    return centerA[0] < centerB[0];
  });

  root = create_bvh_node(shapes);
}

BVHInternalNode::BVHInternalNode(std::span<std::shared_ptr<Shape>> shapes) {
  this->bounding_box = AABB();
  for (auto shape : shapes) {
    this->bounding_box.Expand(shape->getAABB());
  }

  auto minX = shapes.front()->getAABB().pMin[0],
       maxX = shapes.back()->getAABB().pMax[0];
  auto splitX = (minX + maxX) / 2;

  for (auto shape_it = shapes.begin() + 1; shape_it != shapes.end();
       ++shape_it) {
    auto shape = *shape_it;
    if (shape->getAABB().Center()[0] >= splitX ||
        shape_it + 1 == shapes.end()) {
      left = create_bvh_node(std::span(shapes.begin(), shape_it));
      right = create_bvh_node(std::span(shape_it, shapes.end()));
      break;
    }
  }

  assert(left != nullptr && right != nullptr);
}

bool BVHNode_rayIntersect(BVH::BVHNode *root, Ray &ray, int *geomID,
                          int *primID, float *u, float *v) {
  return match(
      *root,
      [&](BVHLeaf leaf) {
        for (auto shape : leaf.shapes) {
          if (shape->rayIntersectShape(ray, primID, u, v)) {
            *geomID = shape->geometryID;
            return true;
          }
        }
        return false;
      },
      [&](BVHInternalNode node) {
        if (ray.direction[0] >= 0) {
          // Positive on X direction
          // Will check left child first
          auto intersect_left =
              BVHNode_rayIntersect(node.left, ray, geomID, primID, u, v);
          auto intersect_right =
              BVHNode_rayIntersect(node.right, ray, geomID, primID, u, v);
          return intersect_left || intersect_right;
        } else {
          // Negative on X direction
          // Will check right child first
          auto intersect_right =
              BVHNode_rayIntersect(node.right, ray, geomID, primID, u, v);
          auto intersect_left =
              BVHNode_rayIntersect(node.left, ray, geomID, primID, u, v);
          return intersect_right || intersect_left;
        }
      });
}

bool BVH::rayIntersect(Ray &ray, int *geomID, int *primID, float *u,
                       float *v) const {
  return BVHNode_rayIntersect(root, ray, geomID, primID, u, v);
}
