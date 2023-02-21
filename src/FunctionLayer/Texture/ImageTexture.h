#pragma once

#include "Texture.h"
#include <CoreLayer/ColorSpace/Spectrum.h>
#include <ResourceLayer/Image.h>
class ImageTexture : public Texture<Spectrum> {
public:
  ImageTexture() = delete;

  ImageTexture(const Json &json);

  virtual Spectrum evaluate(const TextureCoord &texCoord) const override;

  virtual Spectrum lookUp(Vector2i xy) const override;

private:
  std::shared_ptr<Image> image;
};
