#include "NormalTexture.h"
#include <ResourceLayer/FileUtil.h>
NormalTexture::NormalTexture(const Json &json) {
  std::string relativePath = fetchRequired<std::string>(json, "file");
  std::string fullPath = FileUtil::getFullPath(relativePath);
  normalmap = loadImage(fullPath.c_str());
  size = normalmap->size;
}

// TODO 目前法线贴图没有任何过滤
// 返回TBN坐标系下法线的表示
Vector3f NormalTexture::evaluate(const Intersection &intersection) const {
  TextureCoord texCoord = mapping->map(intersection);
  return evaluate(texCoord);
}

Vector3f NormalTexture::evaluate(const TextureCoord &texCoord) const {
  int x = texCoord.coord[0] * size[0], y = texCoord.coord[1] * size[1];
  x = clamp(x, 0, size[0] - 1);
  y = clamp(y, 0, size[1] - 1);
  Vector3f xzy = normalmap->getValue({x, y});
  return xzy * 2.f - Vector3f(1.f);
}