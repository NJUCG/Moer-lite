#pragma once
#include <CoreLayer/ColorSpace/Spectrum.h>
#include <ResourceLayer/Image.h>

class Film {
public:
  Film() = delete;

  Film(Vector2i _size) : size(_size) { image = std::make_shared<Image>(size); }

  void deposit(const Vector2i xy, const Spectrum &spectrum) {
    //* 无论光谱内部实现如何，写入图片时均转为3通道格式
    Vector3f v = toVec3(spectrum);
    image->set(xy, v);
  }

  void savePNG(const char *filename) { image->savePNG(filename); }

public:
  Vector2i size;

protected:
  std::shared_ptr<Image> image = nullptr;
};