#pragma once
#include <CoreLayer/Math/Math.h>
#include <memory>

//* 目前只支持三通道（即RGB）的图片格式

//* Image的数据组织格式如下
//* [0, 0] ----X-----
//* |               |
//* |               |
//* Y        [x, y] |
//* -----------------
//* 坐标[0, 0]对应图片左上角
//* 坐标[x, y]对应图片第x列，第y行

class Image {
public:
  Image() = delete;

  Image(Vector2i _size) : size(_size) {
    data = new float[_size[0] * _size[1] * channels];
  }

  Image(Vector2i _size, float *_data) : size(_size), data(_data) {}

  ~Image() { delete[] data; }

  //* 获取坐标[x, y]处的三通道值
  Vector3f get(const Vector2i &xy) const;

  //* 设置坐标[x, y]处的三通道值
  void set(const Vector2i &xy, const Vector3f &val);

  //* 以PNG格式保存该图片
  void savePNG(const char *filename) const;

public:
  Vector2i size;
  static constexpr int channels = 3;

private:
  float *data = nullptr;
};

//* 根据路径加载一张图片(PNG/JPG)
std::shared_ptr<Image> loadImage(const char *filepath);
