#include "ImageTexture.h"
#include <ResourceLayer/Factory.h>

ImageTexture::ImageTexture(const Json &json) : Texture<Spectrum>() {
  //  std::string filepath = fetchRequired<std::string>(json, "file");
  //  filepath = FileUtil::getFullPath(filepath);
  //  image = loadImage(filepath.c_str());
  image = loadImage(
      "/home/zcx/Programming/Moer-lite/scenes/bunny/textures/tex.jpg");
  size = image->size;
}

//! 因为texture的坐标是从图片左下角为[0,0]点，故需要将y做翻转，否则渲染出的纹理是上下颠倒的
Spectrum ImageTexture::evaluate(Vector2f uv) const {
  int x = uv[0] * size[0], y = (1.f - uv[1]) * size[1];
  return Spectrum(image->get({x, y}));
}

REGISTER_CLASS(ImageTexture, "imageTex")