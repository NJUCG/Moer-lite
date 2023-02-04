#pragma once
#include "Ray.h"
#include <FunctionLayer/Film/Film.h>

struct CameraSample {
  Vector2f xy;
  Vector2f lens;
  float time;
};

//* 所有相机的基类
class Camera : public Transformable {
public:
  Camera() = delete;

  Camera(float _tNear, float _tFar, float _timeStart, float _timeEnd,
         std::shared_ptr<Film> _film)
      : Transformable(), tNear(_tNear), tFar(_tFar), timeStart(_timeStart),
        timeEnd(_timeEnd), film(_film) {}

  //* sampleRay接受一个随机采样sample（包含5个随机数），产生一条光线
  //* NDC是Normalized Device Coordinate
  //* 其范围[0, 0] - [1, 1], NDC与相机Film的对应如下
  //*     ----------[1, 1]
  //*     Y           |
  //*     |           |
  //*   [0, 0]----X----
  virtual Ray sampleRay(const CameraSample &sample, Vector2f NDC) const = 0;

protected:
  //* 可视最近距离、最远距离、快门开启时间和结束时间
  float tNear, tFar, timeStart, timeEnd;
  std::shared_ptr<Film> film = nullptr;
};

//* 透视相机
//* 透视相机使得渲染出的图片拥有近大远小的特点
//* 创建一个透视相机需要指定相机的位置、指向的点以及向上方向
//* 透视相机还可以分为针孔相机、薄透镜相机两种，因此透视相机仍然是一个抽象类
class PerspectiveCamera : public Camera {
public:
  PerspectiveCamera() = delete;

  PerspectiveCamera(const Point3f &position, const Point3f &lookAt,
                    const Vector3f &up);

  virtual Ray sampleRay(const CameraSample &, Vector2f) const = 0;
};