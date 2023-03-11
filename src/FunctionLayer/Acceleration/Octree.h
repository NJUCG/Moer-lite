#pragma once
#include "Acceleration.h"

// MAX_LEAF_SIZE表示叶节点中至多包含的shape的个数
// 若构建时，某个节点的Shape个数多于MAX_LEAF_SIZE，那么就会对该节点继续划分

#define MAX_LEAF_SIZE 64

struct OctreeNode {
  AABB boundingBox; // 该节点的包围盒

  std::shared_ptr<OctreeNode> subNodes[8]; // 该节点的八个子节点

  // primCount设为-1时表示该节点不是叶节点，primCount为正值时表示叶节点有多少个shape
  int primCount = -1;

  int primIdxBuffer[MAX_LEAF_SIZE];
};

//* 八叉树 Octree

class OctreeAcceleration : public Acceleration {
public:
  OctreeAcceleration();

  virtual ~OctreeAcceleration();

  virtual void build() override;

  virtual bool rayIntersect(Ray &ray, int *geomID, int *primID, float *u,
                            float *v) const override;

protected:
  std::shared_ptr<OctreeNode>
  recursiveBuild(const AABB &aabb, const std::vector<int> &primIdxBuffer);

protected:
  std::shared_ptr<OctreeNode> root;
};
