#pragma once
#include <CoreLayer/ColorSpace/Spectrum.h>
#include <ResourceLayer/Image.h>
#include <ResourceLayer/JsonUtil.h>

class Film {
public:
  Film() = delete;

  Film(const Json &json) {
    size = fetchRequired<Vector2i>(json, "size");
    image = std::make_shared<Image>(size);
  }

  void deposit(const Vector2i xy, const Spectrum &spectrum) {
    //* 无论光谱内部实现如何，写入图片时均转为3通道格式
    Vector3f v = toVec3(spectrum);
    image->setValue(xy, v);
  }

  void savePNG(const char *filename) { image->savePNG(filename); }

  void saveHDR(const char *filename) { image->saveHDR(filename); }

public:
  Vector2i size;

protected:
  std::shared_ptr<Image> image = nullptr;
};