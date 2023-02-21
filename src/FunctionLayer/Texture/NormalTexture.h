#pragma once

#include "Texture.h"
#include <ResourceLayer/Image.h>

class NormalTexture : public Texture<Vector3f> {
public:
  NormalTexture() = delete;

  NormalTexture(const Json &json);

  virtual Vector3f evaluate(const TextureCoord &texCoord) const override;

  virtual Vector3f lookUp(Vector2i xy) const override;

private:
  std::shared_ptr<Image> normalmap;
};