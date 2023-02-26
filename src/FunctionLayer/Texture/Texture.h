#pragma once

#include <CoreLayer/Math/Math.h>
#include <FunctionLayer/Shape/Shape.h>
#include <ResourceLayer/FileUtil.h>
#include <ResourceLayer/JsonUtil.h>
// TODO 目前只考虑二维纹理
// TODO 目前不使用模板对Texture做进一步的抽象
struct Intersection;

struct TextureCoord {
  Vector2f coord;
  Vector2f duvdx{.0f, .0f};
  Vector2f duvdy{.0f, .0f};
};

class TextureMapping {
public:
  virtual TextureCoord map(const Intersection &intersection) const = 0;
};

class UVMapping : public TextureMapping {
public:
  UVMapping() = default;

  virtual TextureCoord map(const Intersection &intersection) const override;
};

template <typename TReturn> class Texture {
public:
  Texture() {
    //* 默认使用UV mapping
    mapping = std::make_shared<UVMapping>();
  }

  virtual TReturn evaluate(const Intersection &intersection) const = 0;

  virtual TReturn evaluate(const TextureCoord &texCoord) const = 0;

public:
  Vector2i size;

protected:
  std::shared_ptr<TextureMapping> mapping;
};
