#pragma once
#include "Ray.h"
#include <FunctionLayer/Film/Film.h>

struct CameraSample {
  Vector2f xy;
  Vector2f lens;
  float time;
};

class Camera : public Transformable {
public:
  Camera() = delete;

  Camera(float _tNear, float _tFar, float _timeStart, float _timeEnd,
         std::shared_ptr<Film> _film)
      : Transformable(), tNear(_tNear), tFar(_tFar), timeStart(_timeStart),
        timeEnd(_timeEnd), film(_film) {}

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
class PerspectiveCamera : public Camera {
public:
protected:
};