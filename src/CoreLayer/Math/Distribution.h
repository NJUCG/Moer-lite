#pragma once
#include <functional>
#include <vector>

template <typename T> class Distribution {
public:
  Distribution() = default;

  Distribution(const std::vector<T> &_data,
               const std::function<float(T)> &weightFunction) {
    data = _data;
    cdf.reserve(data.size() + 1);
    cdf.emplace_back(.0f);

    for (int i = 0; i < data.size(); ++i) {
      float weight = weightFunction(data[i]);
      cdf.emplace_back(weight + cdf.back());
    }

    float invTotal = 1.f / cdf.back();
    for (int i = 0; i < cdf.size(); ++i) {
      cdf[i] *= invTotal;
    }
  }

  T sample(float sample, float *pdf) const {
    if (cdf.size() == 1) {
      // no data in distribution
      *pdf = .0f;
      return T();
    }
    auto entry = std::lower_bound(cdf.cbegin(), cdf.cend(), sample);
    size_t index = entry - cdf.cbegin() - 1;
    *pdf = cdf[index + 1] - cdf[index];
    return data[std::min(index, cdf.size() - 2)];
  }

  float pdf(T sampled) const {
    auto entry = std::find(data.cbegin(), data.cend(), sampled);
    if (entry == data.cend())
      return .0f;
    size_t index = entry - data.cbegin();
    return cdf[index + 1] - cdf[index];
  }

private:
  std::vector<float> cdf;
  std::vector<T> data;
};