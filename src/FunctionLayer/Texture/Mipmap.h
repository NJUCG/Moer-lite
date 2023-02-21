#pragma once

#include <ResourceLayer/Image.h>

// TODO 目前只支持对长宽相等且均为2的次幂的图片做mipmap
class MipMap {
public:
  MipMap(std::shared_ptr<Image> origin);

  Vector3f lookUp(Vector2f uv, Vector2f duv0, Vector2f duv1) const;

private:
  std::vector<std::shared_ptr<Image>> pyramid;
};