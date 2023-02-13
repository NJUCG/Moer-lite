#pragma once

#include <CoreLayer/Math/Math.h>
#include <ResourceLayer/FileUtil.h>
#include <ResourceLayer/JsonUtil.h>

// TODO 目前只考虑二维纹理

struct TextureCoord {
  Vector2f coord;
  Vector2f dcdx;
  Vector2f dcdy;
};

template <typename Tvalue> class Texture {
public:
  virtual Tvalue evaluate(const TextureCoord &texCoord) const = 0;

  Vector2i size;
};
