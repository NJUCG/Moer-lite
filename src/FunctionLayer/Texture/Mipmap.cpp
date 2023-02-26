#include "Mipmap.h"

bool isPowerOf2(int v) { return v && !(v & (v - 1)); }

int log2Int(int v) {
  int res = -1;
  do {
    res++;
    v /= 2;
  } while (v);
  return res;
}

std::shared_ptr<Image> resizeHalf(std::shared_ptr<Image> origin) {
  int newX = origin->size[0] / 2, newY = origin->size[1] / 2;
  newX = std::max(newX, 1);
  newY = std::max(newY, 1);
  std::shared_ptr<Image> newImage =
      std::make_shared<Image>(Vector2i{newX, newY});
  for (int y = 0; y < newY; ++y) {
    for (int x = 0; x < newX; ++x) {
      Vector3f v1 = origin->getValue({x * 2, y * 2}),
               v2 = origin->getValue({x * 2 + 1, y * 2}),
               v3 = origin->getValue({x * 2, y * 2 + 1}),
               v4 = origin->getValue({x * 2 + 1, y * 2 + 1});
      newImage->setValue({x, y}, (v1 + v2 + v3 + v4) * .25f);
    }
  }
  return newImage;
}

MipMap::MipMap(std::shared_ptr<Image> origin) {
  Vector2i size = origin->size;
  if (!isPowerOf2(size[0])) {
    std::cerr << "目前只支持对长宽为2的次幂的图片做mipmap\n";
    exit(1);
  }
  int nLevels = 1 + log2Int(std::max(size[0], size[1]));
  pyramid.reserve(nLevels);
  pyramid.emplace_back(origin);
  for (int i = 1; i < nLevels; ++i) {
    auto previous = pyramid.back();
    auto current = resizeHalf(previous);
    pyramid.emplace_back(current);
  }
}

Vector3f MipMap::texel(int level, int x, int y) const {
  auto image = pyramid[level];
  x = clamp(x, 0, image->size[0] - 1);
  y = clamp(y, 0, image->size[1] - 1);
  return image->getValue(Vector2i{x, y});
}

Vector3f MipMap::bilinear(int level, Vector2f uv) const {
  level = clamp(level, 0, (int)pyramid.size() - 1);
  Vector2i size = pyramid[level]->size;
  float x = uv[0] * size[0] - .5f;
  float y = uv[1] * size[1] - .5f;
  int x0 = std::floor(x);
  int y0 = std::floor(y);
  float dx = x - x0;
  float dy = y - y0;

  return (1 - dx) * (1 - dy) * texel(level, x0, y0) +
         (1 - dx) * dy * texel(level, x0, y0 + 1) +
         dx * (1 - dy) * texel(level, x0 + 1, y0) +
         dx * dy * texel(level, x0 + 1, y0 + 1);
}

Vector3f MipMap::lookUp(Vector2f uv, Vector2f duv0, Vector2f duv1) const {
  float width = std::max(std::max(std::abs(duv0[0]), std::abs(duv0[1])),
                         std::max(std::abs(duv1[0]), std::abs(duv1[1])));
  float level = pyramid.size() - 1 + fm::log2(std::max(width, 1e-8f));
  int x = uv[0] * pyramid[0]->size[0];
  int y = uv[1] * pyramid[0]->size[1];
  // return texel(0, x, y); // force no filter
  return bilinear(0, uv); // force bilinear

  if (level < 0) {
    return bilinear(0, uv);
  } else if (level >= pyramid.size() - 1) {
    return texel(pyramid.size() - 1, 0, 0);
  } else {
    int iLevel = std::floor(level);
    float dl = level - iLevel;
    return (1 - dl) * bilinear(iLevel, uv) + dl * bilinear(iLevel + 1, uv);
  }
}