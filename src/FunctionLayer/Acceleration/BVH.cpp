#include "BVH.h"
struct  BVH::BVHNode{
    //* todo BVH节点结构设计
};
void BVH::build() {
    AABB sceneBox;
    for (const auto & shape : shapes) {
        //* 自行实现的加速结构请务必对每个shape调用该方法，以保证TriangleMesh构建内部加速结构
        //* 由于使用embree时，TriangleMesh::getAABB不会被调用，因此出于性能考虑我们不在TriangleMesh
        //* 的构造阶段计算其AABB，因此当我们将TriangleMesh的AABB计算放在TriangleMesh::initInternalAcceleration中
        //* 所以请确保在调用TriangleMesh::getAABB之前先调用TriangleMesh::initInternalAcceleration
        shape->initInternalAcceleration();
        sceneBox.Expand(shape->getAABB());
    }
    //* todo 完成BVH构建
}
bool BVH::rayIntersect(Ray &ray, int *geomID, int *primID, float *u, float *v) const {
    //* todo 完成BVH求交
    return false;
}


