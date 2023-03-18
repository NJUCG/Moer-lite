#include "Octree.h"
#include <queue>
struct Octree::OctreeNode {
    AABB boundingBox;
    std::shared_ptr<OctreeNode> subNodes[8];
    int primCount = -1;
    int primIdxBuffer[ocLeafMaxSize];
};
Octree::OctreeNode * Octree::recursiveBuild(const AABB &aabb,
                       const std::vector<int> &primIdxBuffer) {
  //* todo 完成递归构建八叉树
  //* 构建方法请看实验手册
  //* 要注意的一种特殊是当节点的某个子包围盒和当前节点所有物体都相交，我们就不用细分了，当前节点作为叶子节点即可。
    return nullptr;
}
void Octree::build() {
  //* 首先计算整个场景的范围
  for (const auto & shape : shapes) {
    //* 自行实现的加速结构请务必对每个shape调用该方法，以保证TriangleMesh构建内部加速结构
    //* 由于使用embree时，TriangleMesh::getAABB不会被调用，因此出于性能考虑我们不在TriangleMesh
    //* 的构造阶段计算其AABB，因此当我们将TriangleMesh的AABB计算放在TriangleMesh::initInternalAcceleration中
    //* 所以请确保在调用TriangleMesh::getAABB之前先调用TriangleMesh::initInternalAcceleration
    shape->initInternalAcceleration();

    boundingBox.Expand(shape->getAABB());
  }

  //* 构建八叉树
  std::vector<int> primIdxBuffer(shapes.size());
  std::iota(primIdxBuffer.begin(), primIdxBuffer.end(), 0);
  root = recursiveBuild(boundingBox, primIdxBuffer);
}

bool Octree::rayIntersect(Ray &ray, int *geomID, int *primID,
                          float *u, float *v) const {
  //*todo 完成八叉树求交
  return false;
}