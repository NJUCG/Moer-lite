#include "Image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
Vector3f Image::get(const Vector2i &xy) const {
  int offset = (xy[0] + xy[1] * size[0]) * channels;
  return Vector3f(data[offset], data[offset + 1], data[offset + 2]);
}

void Image::set(const Vector2i &xy, const Vector3f &val) {
  int offset = (xy[0] + xy[1] * size[0]) * channels;
  for (int i = 0; i < 3; ++i) {
    data[offset + i] = val[i];
  }
}

void Image::savePNG(const char *filename) const {
  uint8_t *result = new uint8_t[size[0] * size[1] * channels]();
  for (int i = 0; i < size[0] * size[1] * channels; ++i) {
    result[i] = clamp<uint8_t>(data[i] * 255, 0, 255);
  }
  stbi_write_png(filename, size[0], size[1], 3, result, 0);
  delete[] result;
}

std::shared_ptr<Image> loadImage(const char *filepath) {
  uint8_t *datau = nullptr;
  int x, y, channels;
  datau = stbi_load(filepath, &x, &y, &channels, 3);

  // TODO support HDR
  if (stbi_is_hdr(filepath)) {
    printf("Hdr not support now!\n");
    exit(1);
  }
  if (!datau) {
    printf("Load %s failed\n", filepath);
    exit(1);
  }
  if (channels != 3) {
    printf("Load %s failed, %d channels found\n", filepath, channels);
    exit(1);
  }

  float *dataf = new float[x * y * 3];
  auto convert = [](uint8_t u8) -> float { return u8 / 255.f; };
  for (int i = 0; i < x * y * 3; ++i) {
    dataf[i] = convert(datau[i]);
  }
  stbi_image_free(datau);
  return std::make_shared<Image>(Vector2i(x, y), dataf);
}