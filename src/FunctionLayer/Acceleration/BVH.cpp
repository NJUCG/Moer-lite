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
  if (shapes.size() <= 8)
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

  root = create_bvh_node(shapes);
}

std::pair<float, float>
get_span(const std::span<std::shared_ptr<Shape>> &shapes, int axis) {
  auto minimum =
      *std::min_element(shapes.begin(), shapes.end(), [&](auto a, auto b) {
        return a->getAABB().pMin[axis] < b->getAABB().pMin[axis];
      });
  auto maximum =
      *std::max_element(shapes.begin(), shapes.end(), [&](auto a, auto b) {
        return a->getAABB().pMax[axis] < b->getAABB().pMax[axis];
      });
  return {minimum->getAABB().pMin[axis], maximum->getAABB().pMax[axis]};
}

// Choose the axis where shapes are most spread out
int get_best_axis(const std::span<std::shared_ptr<Shape>> &shapes) {
  auto [x_min, x_max] = get_span(shapes, 0);
  auto [y_min, y_max] = get_span(shapes, 1);
  auto [z_min, z_max] = get_span(shapes, 2);

  auto x_span = x_max - x_min;
  auto y_span = y_max - y_min;
  auto z_span = z_max - z_min;

  if (x_span >= y_span && x_span >= z_span) {
    return 0;
  } else if (y_span >= z_span && y_span >= x_span) {
    return 1;
  } else {
    return 2;
  }
}

BVHInternalNode::BVHInternalNode(std::span<std::shared_ptr<Shape>> shapes) {
  this->shapes = shapes;
  this->bounding_box = AABB();
  for (auto shape : shapes) {
    this->bounding_box.Expand(shape->getAABB());
  }

  auto axis = get_best_axis(shapes);
  this->splitAxis = axis;
  std::sort(shapes.begin(), shapes.end(), [axis](auto a, auto b) {
    auto centerA = a->getAABB().Center();
    auto centerB = b->getAABB().Center();
    return centerA[axis] < centerB[axis];
  });

  auto minPos = shapes.front()->getAABB().pMin[axis],
       maxPos = shapes.back()->getAABB().pMax[axis];
  auto splitPos = (minPos + maxPos) / 2;

  for (auto shape_it = shapes.begin() + 1; shape_it != shapes.end();
       ++shape_it) {
    auto shape = *shape_it;
    if (shape->getAABB().Center()[axis] >= splitPos ||
        shape_it + 1 == shapes.end()) {
      left = create_bvh_node(std::span(shapes.begin(), shape_it));
      right = create_bvh_node(std::span(shape_it, shapes.end()));
      break;
    }
  }

  assert(left != nullptr && right != nullptr);
}

bool BVHNode_rayIntersect(BVH::BVHNode *root, Ray &ray, int *geomID,
                          int *primID, float *u, float *v, int depth) {
  return match(
      *root,
      [&](BVHLeaf leaf) {
        auto intersected = false;
        for (auto shape : leaf.shapes) {
          if (shape->rayIntersectShape(ray, primID, u, v)) {
            *geomID = shape->geometryID;
            intersected = true;
          }
        }
        return intersected;
      },
      [&](BVHInternalNode node) {
        if (node.bounding_box.RayIntersect(ray) == false)
          return false;

        if (ray.direction[node.splitAxis] >= 0) {
          // Positive on splitted axis
          // Will check left child first
          auto intersect_left = BVHNode_rayIntersect(node.left, ray, geomID,
                                                     primID, u, v, depth + 1);
          auto intersect_right = BVHNode_rayIntersect(node.right, ray, geomID,
                                                      primID, u, v, depth + 1);
          return intersect_left || intersect_right;
        } else {
          // Negative on splitted axis
          // Will check right child first
          auto intersect_right = BVHNode_rayIntersect(node.right, ray, geomID,
                                                      primID, u, v, depth + 1);
          auto intersect_left = BVHNode_rayIntersect(node.left, ray, geomID,
                                                     primID, u, v, depth + 1);
          return intersect_right || intersect_left;
        }
      });
}

bool BVH::rayIntersect(Ray &ray, int *geomID, int *primID, float *u,
                       float *v) const {
  return BVHNode_rayIntersect(root, ray, geomID, primID, u, v, 0);
}
