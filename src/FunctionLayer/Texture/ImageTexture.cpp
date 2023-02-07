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

Spectrum ImageTexture::evaluate(Vector2f uv) const {
  int x = uv[0] * size[0], y = (1.f - uv[1]) * size[1];
  return Spectrum(image->get({x, y}));
}

REGISTER_CLASS(ImageTexture, "imageTex")