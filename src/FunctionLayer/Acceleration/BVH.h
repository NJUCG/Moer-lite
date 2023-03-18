#pragma once
#include "Acceleration.h"
class BVH : public Acceleration{
public:
    BVH() = default;
    void build() override;
    bool rayIntersect(Ray &ray, int *geomID, int *primID, float *u, float *v) const override;
protected:
    static constexpr int bvhLeafMaxSize = 64;
    struct BVHNode;
    BVHNode * root;
};