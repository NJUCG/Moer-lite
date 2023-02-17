#pragma once

#include <CoreLayer/Math/Math.h>
#include <FunctionLayer/Shape/Shape.h>
#include <ResourceLayer/FileUtil.h>
#include <ResourceLayer/JsonUtil.h>
// TODO 目前只考虑二维纹理
// TODO 目前不使用模板对Texture做进一步的抽象
struct Intersection;

struct TextureCoord {
  Vector2f coord;
  Vector2f dcdx;
  Vector2f dcdy;
};

template <typename TReturn> class Texture {
public:
  // !封装一下从intersection得到纹理坐标的过程
  // 最终会调用 evaluate(const TextureCoord &)方法
  TReturn eval(const Intersection &intersection) const {
    TextureCoord texCoord;
    texCoord.coord = intersection.texCoord;
    return evaluate(texCoord);
  }

  virtual TReturn evaluate(const TextureCoord &texCoord) const = 0;

public:
  Vector2i size;
};
