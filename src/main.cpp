#include <CoreLayer/Math/Math.h>
#include <FunctionLayer/Camera/Pinhole.h>
#include <ResourceLayer/Factory.h>
#include <ResourceLayer/Image.h>
#include <ResourceLayer/JsonUtil.h>
#include <fstream>
#include <stdio.h>

int main() {
  std::ifstream fstm("/home/zcx/Programming/Moer-lite/scenes/test.json");
  Json json = Json::parse(fstm);
  const auto &camera_type = fetchRequired<std::string>(json["camera"], "type");
  auto camera = Factory::construct_class<Camera>(camera_type, json["camera"]);
  Ray ray = camera->sampleRay(CameraSample(), {.5f, .5f});
  ray.origin.debugPrint();
  ray.direction.debugPrint();
}
