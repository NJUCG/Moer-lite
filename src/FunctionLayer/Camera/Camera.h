#pragma once
#include <FunctionLayer/Film/Film.h>
#include <FunctionLayer/Ray/Ray.h>
#include <ResourceLayer/Factory.h>
#include <ResourceLayer/JsonUtil.h>
#include <CoreLayer/Math/Transform.h>

struct CameraSample {
  Vector2f xy;
  Vector2f lens;
  float time;
};

//* 所有相机的基类
class Camera : public Transformable {
public:
  Camera() = delete;

  Camera(const Json &json);

  //* sampleRay接受一个随机采样sample（包含5个随机数），产生一条光线
  //* NDC是Normalized Device Coordinate
  //* 其范围[0, 0] - [1, 1], NDC与相机Film的对应如下
  //*   [0, 0]----X----
  //*     |           |
  //*     Y           |
  //*     ----------[1, 1]
  virtual Ray sampleRay(const CameraSample &sample, Vector2f NDC) const = 0;

  virtual Ray sampleRayDifferentials(const CameraSample &sample,
                                     Vector2f NDC) const = 0;

  std::shared_ptr<Film> film = nullptr;

protected:
  //* 可视最近距离、最远距离、快门开启时间和结束时间
  float tNear, tFar, timeStart, timeEnd;
};

//* 透视相机
//* 透视相机使得渲染出的图片拥有近大远小的特点
//* 创建一个透视相机需要指定相机的位置、指向的点以及向上方向
//* 透视相机还可以分为针孔相机、薄透镜相机两种，因此透视相机仍然是一个抽象类
class PerspectiveCamera : public Camera {
public:
  PerspectiveCamera() = delete;

  PerspectiveCamera(const Json &json);

  virtual Ray sampleRay(const CameraSample &, Vector2f) const = 0;

protected:
  float verticalFov, aspectRatio;
};