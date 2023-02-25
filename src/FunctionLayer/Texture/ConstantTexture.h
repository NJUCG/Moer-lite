#pragma once

#include "Texture.h"
#include <CoreLayer/ColorSpace/Spectrum.h>
template <typename TReturn> class ConstantTexture : public Texture<TReturn> {
public:
  ConstantTexture(const TReturn &_data) : data(_data) {}

  virtual TReturn evaluate(const Intersection &intersection) const override {
    return data;
  }

  virtual TReturn evaluate(const TextureCoord &texCoord) const override {
    return data;
  }

private:
  TReturn data;
};