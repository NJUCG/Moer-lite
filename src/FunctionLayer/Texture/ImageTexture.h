#pragma once

#include "Texture.h"
#include <CoreLayer/ColorSpace/Spectrum.h>
#include <ResourceLayer/Image.h>
class ImageTexture : public Texture<Spectrum> {
public:
  ImageTexture() = delete;

  ImageTexture(const Json &json);

  virtual Spectrum evaluate(const TextureCoord &texCoord) const override;

private:
  std::vector<std::shared_ptr<Image>> mipmap;
  Vector2i size;
};