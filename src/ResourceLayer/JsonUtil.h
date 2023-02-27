#pragma once
#include <CoreLayer/ColorSpace/RGB.h>
#include <CoreLayer/Math/Math.h>
#include <iostream>
#include <nlohmann/json.hpp>
using Json = nlohmann::json;

//* 返回Json中名为filedName，类型为TargetType的值，如果Json中没有这个值，则返回defaultValue
template <typename TargetType>
TargetType fetchOptional(const Json &json, const std::string &fieldName,
                         TargetType defaultValue) {
  if (!json.contains(fieldName))
    return defaultValue;
  return json[fieldName].get<TargetType>();
}

//* 返回Json中名为fieldName，类型为TargetType的值，如果Json中没有这个值则报错
template <typename TargetType>
TargetType fetchRequired(const Json &json, const std::string &fieldName) {
  if (!json.contains(fieldName)) {
    std::cerr << fieldName << " is required but not exists!\n";
    std::cerr << "In " << json << std::endl;
    std::exit(1);
  }
  return json[fieldName].get<TargetType>();
}

namespace vecmat { //* 将一个json对象转换为Vector2i
inline void from_json(const Json &json, vec<2, int> &v) {
  if (!json.is_array() || json.size() != 2) {
    std::cerr << "Json can't parse to Vector2i\n";
    exit(1);
  }
  json[0].get_to<int>(v[0]);
  json[1].get_to<int>(v[1]);
}

//* 将一个json对象转换为Vector2f
inline void from_json(const Json &json, vec<2, float> &v) {
  if (!json.is_array() || json.size() != 2) {
    std::cerr << "Json can't parse to Vector2f\n";
    exit(1);
  }
  json[0].get_to<float>(v[0]);
  json[1].get_to<float>(v[1]);
}
}; // namespace vecmat

//* 将一个json对象转换为Vector3f
inline void from_json(const Json &json, Vector3f &v) {
  if (!json.is_array() || json.size() != 3) {
    std::cerr << "Json can't parse to Vector3f\n";
    exit(1);
  }
  json[0].get_to<float>(v[0]);
  json[1].get_to<float>(v[1]);
  json[2].get_to<float>(v[2]);
}

//* 将一个json对象转换为Point3f
inline void from_json(const Json &json, Point3f &p) {
  if (!json.is_array() || json.size() != 3) {
    std::cerr << "Json can't parse to Vector3f\n";
    exit(1);
  }
  json[0].get_to<float>(p[0]);
  json[1].get_to<float>(p[1]);
  json[2].get_to<float>(p[2]);
}

//* 将一个json对象转换为Spectrum
inline void from_json(const Json &json, SpectrumRGB &s) {
  if (!json.is_array() || json.size() != 3) {
    std::cerr << "Json can't parse to SpectrumRGB\n";
    exit(1);
  }
  json[0].get_to<float>(s[0]);
  json[1].get_to<float>(s[1]);
  json[2].get_to<float>(s[2]);
}