#pragma  once
#include "Shape.h"

class Disk : public  Shape{
public:
    bool rayIntersectShape(Ray &ray, int *primID, float *u, float *v) const override;
    void fillIntersection(float distance, int primID, float u, float v, Intersection *intersection) const override;
    void uniformSampleOnSurface(Vector2f sample, Intersection *result, float *pdf) const override;
    Disk(const Json & json);
private:
    //* 圆环的外半径和内半径
    float radius,innerRadius;
    //* 圆环的最大角度
    float phiMax;
};