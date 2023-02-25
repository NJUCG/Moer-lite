#include "Texture.h"

TextureCoord UVMapping::map(const Intersection &intersection) const {
  return TextureCoord{intersection.texCoord,
                      Vector2f{intersection.dudx, intersection.dvdx},
                      Vector2f{intersection.dudy, intersection.dvdy}};
}