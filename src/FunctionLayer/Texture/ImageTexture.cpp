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

Spectrum ImageTexture::lookUp(Vector2i xy) const {
  xy[0] = clamp(xy[0], 0, size[0] - 1);
  xy[1] = clamp(xy[1], 0, size[1] - 1);
  return image->getValue(xy);
}

REGISTER_CLASS(ImageTexture, "imageTex")