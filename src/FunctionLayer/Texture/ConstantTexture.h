#pragma once

#include "Texture.h"
#include <CoreLayer/ColorSpace/Spectrum.h>
template <typename Tvalue> class ConstantTexture : public Texture<Tvalue> {
public:
  ConstantTexture(const Tvalue &_data) : data(_data) {}

  virtual Tvalue evaluate(const TextureCoord &texCoord) const override {
    return data;
  }

private:
  Tvalue data;
};