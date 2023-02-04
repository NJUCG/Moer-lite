#pragma once
#include <iostream>
#include <nlohmann/json.hpp>

using Json = nlohmann::json;

//* 返回Json对象中名为filedName，类型为TargetType的值，如果Json对象中没有这个值，则返回defaultValue
template <typename TargetType>
TargetType fetchOptional(const Json &json, const std::string &fieldName,
                         TargetType defaultValue) {
  if (!json.contains(fieldName))
    return defaultValue;
  return json[fieldName].get<TargetType>();
}

//* 返回Json对象中名为fieldName，类型为TargetType的值，如果Json对象中没有这个值则报错
template <typename TargetType>
TargetType fetchRequired(const nlohmann::json &json,
                         const std::string &fieldName) {
  if (!json.contains(fieldName)) {
    std::cerr << fieldName << " is required but not exists!\n";
    std::exit(1);
  }
  return json[fieldName].get<TargetType>();
}
