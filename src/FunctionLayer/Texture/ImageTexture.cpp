#include "ImageTexture.h"
#include <ResourceLayer/Factory.h>

bool isPowerOf2(int i) { return i && !(i & (i - 1)); }
int log2Int(int i) {
  int res = -1;
  while (i) {
    i = i >> 1;
    res++;
  }
  return res;
}
//* 将原图片长宽各缩小为一半，将一个2x2的像素矩阵缩小为1个像素
std::shared_ptr<Image> shrinkHalf(std::shared_ptr<Image> origin) {
  std::shared_ptr<Image> half = std::make_shared<Image>(Vector2i{
      std::max(origin->size[0] / 2, 1), std::max(origin->size[1] / 2, 1)});
  for (int y = 0; y < half->size[1]; ++y) {
    for (int x = 0; x < half->size[0]; ++x) {
      if (half->size[0] == 1) {
        Vector3f v0 = origin->getValue(Vector2i{x * 2, y * 2}),
                 v1 = origin->getValue(Vector2i{x * 2, y * 2 + 1});
        half->setValue({x, y}, (v0 + v1) * .5f);
      } else if (half->size[1] == 0) {
        Vector3f v0 = origin->getValue(Vector2i{x * 2, y * 2}),
                 v1 = origin->getValue(Vector2i{x * 2 + 1, y * 2});
        half->setValue({x, y}, (v0 + v1) * .5f);
      } else {
        Vector3f v0 = origin->getValue(Vector2i{x * 2, y * 2}),
                 v1 = origin->getValue(Vector2i{x * 2 + 1, y * 2}),
                 v2 = origin->getValue(Vector2i{x * 2, y * 2 + 1}),
                 v3 = origin->getValue(Vector2i{x * 2 + 1, y * 2 + 1});
        half->setValue({x, y}, (v0 + v1 + v2 + v3) * 0.25f);
      }
    }
  }
  return half;
}

ImageTexture::ImageTexture(const Json &json) : Texture<Spectrum>() {
  //  std::string filepath = fetchRequired<std::string>(json, "file");
  //  filepath = FileUtil::getFullPath(filepath);
  //  image = loadImage(filepath.c_str());
  mipmap.emplace_back(loadImage(
      "/home/zcx/Programming/Moer-lite/scenes/bunny/textures/tex.jpg"));
  size = mipmap[0]->size;

  // TODO 目前只支持对长、宽为2的次幂的图片做mipmap
  if (!isPowerOf2(size[0]) || !isPowerOf2(size[1])) {
    std::cerr << "Mipmap只支持长、宽为2的次幂的图片\n";
  } else {
    //* 构建mipmap
    int nLevels = 1 + log2Int(std::max(size[0], size[1]));
    for (int i = 1; i < nLevels; ++i) {
      auto prev = mipmap.back();
      mipmap.emplace_back(shrinkHalf(prev));
    }
  }
}

//! 因为texture的坐标是从图片左下角为[0,0]点，故需要将y做翻转，否则渲染出的纹理是上下颠倒的
Spectrum ImageTexture::evaluate(const TextureCoord &texCoord) const {
  int x = texCoord.coord[0] * mipmap[0]->size[0],
      y = (1.f - texCoord.coord[1]) * mipmap[0]->size[1];
  return mipmap[0]->getValue({x, y});
}

REGISTER_CLASS(ImageTexture, "imageTex")