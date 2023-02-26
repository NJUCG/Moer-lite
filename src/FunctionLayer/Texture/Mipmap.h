#pragma once

#include <ResourceLayer/Image.h>

// TODO 目前只支持对长宽均为2的次幂的图片做mipmap
class MipMap {
public:
  MipMap(std::shared_ptr<Image> origin);

  Vector3f texel(int level, int x, int y) const;

  Vector3f bilinear(int level, Vector2f uv) const;

  Vector3f lookUp(Vector2f uv, Vector2f duvdx, Vector2f duvdy) const;

private:
  std::vector<std::shared_ptr<Image>> pyramid;
};