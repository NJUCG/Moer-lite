#pragma once
#include "JsonUtil.h"
#include <map>

#define REGISTER_CLASS(CLASS, CLASS_NAME)                                      \
  std::shared_ptr<CLASS> CLASS##_helper(const Json &json) {                    \
    return std::make_shared<CLASS>(json);                                      \
  }                                                                            \
  static struct CLASS##_ {                                                     \
    CLASS##_() {                                                               \
      if (Factory::getMap().count(CLASS_NAME) != 0) {                          \
        std::cerr << "Fatal, multiple definition class " << CLASS_NAME         \
                  << std::endl;                                                \
        std::exit(1);                                                          \
      }                                                                        \
      Factory::register_class(CLASS_NAME, CLASS##_helper);                     \
    }                                                                          \
  } CLASS##__;

class Factory {
public:
  Factory() = delete;

  static void
  register_class(const std::string &name,
                 const std::function<std::shared_ptr<void>(const Json &json)>
                     &constructor) {
    // constructor_map[name] = constructor;
    getMap().emplace(std::make_pair(name, constructor));
  }

  template <typename T>
  static std::shared_ptr<T> construct_class(const Json &json) {
    std::string type = fetchRequired<std::string>(json, "type");
    if (getMap().count(type) == 0) {
      std::cerr << "Fatal, unknown type " << type << std::endl;
      std::exit(1);
    }
    return std::static_pointer_cast<T>(getMap()[type](json));
  }

  static std::map<std::string,
                  std::function<std::shared_ptr<void>(const Json &)>> &
  getMap() {
    static std::map<std::string,
                    std::function<std::shared_ptr<void>(const Json &)>>
        constructor_map = {};
    return constructor_map;
  }
};