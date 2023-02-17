#include "ImageTexture.h"
#include <ResourceLayer/Factory.h>

ImageTexture::ImageTexture(const Json &json) : Texture<Spectrum>() {
  std::string filepath = fetchRequired<std::string>(json, "file");
  filepath = FileUtil::getFullPath(filepath);
  image = loadImage(filepath.c_str());
  size = image->size;
}

Spectrum ImageTexture::evaluate(const TextureCoord &texCoord) const {
  int x = texCoord.coord[0] * image->size[0],
      y = texCoord.coord[1] * image->size[1];
  x = clamp(x, 0, size[0] - 1);
  y = clamp(y, 0, size[1] - 1);
  return image->getValue({x, y});
}

REGISTER_CLASS(ImageTexture, "imageTex")