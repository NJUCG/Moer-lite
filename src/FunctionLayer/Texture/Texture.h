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
  Vector2f dcdx;
  Vector2f dcdy;
};

template <typename TReturn> class Texture {
public:
  // !封装一下从intersection得到纹理坐标的过程
  // 最终会调用 evaluate(const TextureCoord &)方法
  TReturn eval(const Intersection &intersection) const {
    TextureCoord texCoord;
    texCoord.coord = intersection.texCoord;
    return evaluate(texCoord);
  }

  virtual TReturn evaluate(const TextureCoord &texCoord) const = 0;

  virtual TReturn lookUp(Vector2i xy) const = 0;

public:
  Vector2i size;
};

// 不做任何过滤的纹理查询
template <typename TReturn>
TReturn query(const std::shared_ptr<Texture<TReturn>> texture,
              const Intersection &intersection) {
  int x = intersection.texCoord[0] * texture->size[0],
      y = intersection.texCoord[1] * texture->size[1];
  return texture->lookUp(Vector2i{x, y});
}

// 双线性过滤插值
template <typename TReturn>
TReturn queryBilinearFiltering(const std::shared_ptr<Texture<TReturn>> texture,
                               const Intersection &intersection) {
  float x = intersection.texCoord[0] * texture->size[0] - .5f,
        y = intersection.texCoord[1] * texture->size[1] - .5f;
  int xMin = std::floor(x), yMin = std::floor(y);

  if (xMin < 0 && yMin < 0) {
    // 纹理边界，无法做双线性过滤
    return texture->lookUp(Vector2i{0, 0});
  } else if (xMin < 0) {
    // 纹理边界，只在y方向做线性过滤
    float w1 = 1.f - (y - yMin);
    TReturn v1 = texture->lookUp(Vector2i{0, yMin});
    float w2 = y - yMin;
    TReturn v2 = texture->lookUp(Vector2i{0, yMin + 1});
    return w1 * v1 + w2 * v2;
  } else if (yMin < 0) {
    // 纹理边界，只在x方向做线性过滤
    float w1 = 1.f - (x - xMin);
    TReturn v1 = texture->lookUp(Vector2i{xMin, 0});
    float w2 = x - xMin;
    TReturn v2 = texture->lookUp(Vector2i{xMin + 1, 0});
    return w1 * v1 + w2 * v2;
  } else {
    // 纹理内部
    float w1 = (1.f - (x - xMin)) * (1.f - (y - yMin));
    TReturn v1 = texture->lookUp(Vector2i{xMin, yMin});
    float w2 = (1.f - (x - xMin)) * (y - yMin);
    TReturn v2 = texture->lookUp(Vector2i{xMin, yMin + 1});
    float w3 = (x - xMin) * (1.f - (y - yMin));
    TReturn v3 = texture->lookUp(Vector2i{xMin + 1, yMin});
    float w4 = (x - xMin) * (y - yMin);
    TReturn v4 = texture->lookUp(Vector2i{xMin + 1, yMin + 1});
    return w1 * v1 + w2 * v2 + w3 * v3 + w4 * v4;
  }
}

// 使用mipmap的三线性过滤插值
template <typename TReturn>
TReturn queryTrilinearFiltering(const std::shared_ptr<Texture<TReturn>> texture,
                                const Intersection &intersection) {
  // TODO
}