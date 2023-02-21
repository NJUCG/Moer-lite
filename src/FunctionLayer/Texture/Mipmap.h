#pragma once

#include <ResourceLayer/Image.h>

// TODO 目前只支持对长宽相等且均为2的次幂的图片做mipmap
class MipMap {
public:
  MipMap(std::shared_ptr<Image> origin);

private:
  std::vector<std::shared_ptr<Image>> pyramid;
};