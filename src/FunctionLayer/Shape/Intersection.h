#pragma once
#include <CoreLayer/Math/Math.h>
#include <FunctionLayer/Ray/Ray.h>

class Shape;

//* Ray与Shape交点处的信息
struct Intersection {
  float distance;              // 从光线起点到交点的距离
  Point3f position;            // 交点的位置
  Vector3f normal;             // 交点处的法线
  Vector3f tangent, bitangent; // 交点处的切线和副切线
  Vector2f texCoord;           // 交点处的纹理坐标
  const Shape *shape;          // 指向与光线相交的物体
  Vector3f dpdu, dpdv;         // 交点处沿u、v方向的切线

  //* 光线微分
  float dudx, dvdx, dudy, dvdy;
  Vector3f dpdx, dpdy;
};

inline void computeRayDifferentials(Intersection *intersection,
                                    const Ray &ray) {
  // 计算光线微分
  do {
    if (ray.hasDifferentials) {
      Point3f p = intersection->position;
      Vector3f n = intersection->normal;
      Vector3f ox = Vector3f{ray.originX[0], ray.originX[1], ray.originX[2]};
      Vector3f oy = Vector3f{ray.originY[0], ray.originY[1], ray.originY[2]};
      float d = dot(n, Vector3f{p[0], p[1], p[2]});
      float tx = -(dot(n, ox) - d) / dot(n, ray.directionX);
      if (std::isinf(tx) || std::isnan(tx))
        break;
      float ty = -(dot(n, oy) - d) / dot(n, ray.directionY);
      if (std::isinf(ty) || std::isnan(ty))
        break;

      Point3f px = ray.origin + tx * ray.directionX;
      Point3f py = ray.origin + ty * ray.directionY;
      intersection->dpdx = px - p;
      intersection->dpdy = py - p;

      int dim[2];
      if (std::abs(n[0]) > std::abs(n[1]) && std::abs(n[0]) > std::abs(n[2])) {
        dim[0] = 1;
        dim[1] = 2;
      } else if (std::abs(n[1]) > std::abs(n[2])) {
        dim[0] = 0;
        dim[1] = 2;
      } else {
        dim[0] = 0;
        dim[1] = 1;
      }

      Vector3f dpdu = intersection->dpdu;
      Vector3f dpdv = intersection->dpdv;
      Vector3f dpdx = intersection->dpdx;
      Vector3f dpdy = intersection->dpdy;
      float A[2][2] = {{dpdu[dim[0]], dpdv[dim[0]]},
                       {dpdu[dim[1]], dpdv[dim[1]]}};
      float Bx[2] = {dpdx[dim[0]], dpdx[dim[1]]};
      float By[2] = {dpdy[dim[0]], dpdy[dim[1]]};

      auto solveLinearSystem2x2 = [](const float A[2][2], const float B[2],
                                     float *x0, float *x1) {
        float det = A[0][0] * A[1][1] - A[0][1] * A[1][0];
        if (std::abs(det) < 1e-10f)
          return false;
        *x0 = (A[1][1] * B[0] - A[0][1] * B[1]) / det;
        *x1 = (A[0][0] * B[1] - A[1][0] * B[0]) / det;
        if (std::isnan(*x0) || std::isnan(*x1))
          return false;
        return true;
      };

      float dudx, dvdx, dudy, dvdy;
      if (!solveLinearSystem2x2(A, Bx, &dudx, &dvdx))
        dudx = dvdx = .0f;
      if (!solveLinearSystem2x2(A, Bx, &dudy, &dvdy))
        dudy = dvdy = .0f;

      intersection->dudx = dudx;
      intersection->dudy = dudy;
      intersection->dvdx = dvdx;
      intersection->dvdy = dvdy;

      return;
    }

  } while (0);

  // 处理特殊情况
  intersection->dudx = .0f;
  intersection->dudy = .0f;
  intersection->dvdx = .0f;
  intersection->dvdy = .0f;
  intersection->dpdx = Vector3f(.0f);
  intersection->dpdy = Vector3f(.0f);
}