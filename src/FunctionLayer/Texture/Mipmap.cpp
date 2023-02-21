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
  if (size[0] != size[1]) {
    std::cerr << "目前只支持对长宽相等的图片做mipmap\n";
    exit(1);
  }
  if (!isPowerOf2(size[0])) {
    std::cerr << "目前只支持对长宽为2的次幂的图片做mipmap\n";
    exit(1);
  }
  int nLevels = 1 + log2Int(size[0]);
  pyramid.reserve(nLevels);
  pyramid.emplace_back(origin);
  for (int i = 1; i < nLevels; ++i) {
    auto previous = pyramid.back();
    auto current = resizeHalf(previous);
    pyramid.emplace_back(current);
  }
}

Vector3f MipMap::lookUp(Vector2f uv, Vector2f duv0, Vector2f duv1) const {
  float width = std::max(std::max(std::abs(duv0[0]), std::abs(duv0[1])),
                         std::max(std::abs(duv1[0]), std::abs(duv1[1])));
  float level = pyramid.size() - 1 + fm::log2(std::max(width, 1e-8f));
}