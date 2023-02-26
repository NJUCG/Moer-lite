#include "Image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
Vector3f Image::getValue(const Vector2i &xy) const {
  int x = clamp(xy[0], 0, size[0] - 1);
  int y = clamp(xy[1], 0, size[1] - 1);
  int offset = (x + y * size[0]) * channels;
  return Vector3f(data[offset], data[offset + 1], data[offset + 2]);
}

void Image::setValue(const Vector2i &xy, const Vector3f &val) {
  int offset = (xy[0] + xy[1] * size[0]) * channels;
  for (int i = 0; i < 3; ++i) {
    data[offset + i] = val[i];
  }
}

void Image::savePNG(const char *filename) const {
  uint8_t *result = new uint8_t[size[0] * size[1] * channels]();
  for (int i = 0; i < size[0] * size[1] * channels; ++i) {
    result[i] = static_cast<uint8_t>(255 * clamp(data[i], .0f, 1.f));
  }
  stbi_write_png(filename, size[0], size[1], 3, result, 0);
  delete[] result;
}

void Image::saveHDR(const char *filename) const {
  stbi_write_hdr(filename, size[0], size[1], 3, data);
}

std::shared_ptr<Image> loadImage(const char *filepath) {
  bool isHDR = stbi_is_hdr(filepath);
  int width, height, channels;
  if (isHDR) {
    float *dataf = stbi_loadf(filepath, &width, &height, &channels, 3);
    if (!dataf) {
      printf("Load %s failed\n", filepath);
      exit(1);
    }
    if (channels != 3) {
      printf("Load %s failed, %d channels found\n", filepath, channels);
      exit(1);
    }
    float *data = new float[width * height * channels];
    memcpy(data, dataf, sizeof(float) * width * height * channels);
    stbi_image_free(dataf);
    return std::make_shared<Image>(Vector2i{width, height}, data);
  } else {
    uint8_t *datau = stbi_load(filepath, &width, &height, &channels, 3);
    if (!datau) {
      printf("Load %s failed\n", filepath);
      exit(1);
    }
    if (channels != 3) {
      printf("Load %s failed, %d channels found\n", filepath, channels);
      exit(1);
    }
    float *data = new float[width * height * 3];
    auto convert = [](uint8_t u8) -> float { return u8 / 255.f; };
    for (int i = 0; i < width * height * 3; ++i) {
      data[i] = convert(datau[i]);
    }
    stbi_image_free(datau);
    return std::make_shared<Image>(Vector2i{width, height}, data);
  }
}