#pragma once

#include <CoreLayer/Math/Math.h>
#include <ResourceLayer/FileUtil.h>
#include <ResourceLayer/JsonUtil.h>

// TODO 目前只考虑二维纹理
template <typename Tvalue> class Texture {
public:
  // TODO 增加纹理过滤需要的函数
  virtual Tvalue evaluate(Vector2f uv) const = 0;

public:
  Vector2i size;
};
