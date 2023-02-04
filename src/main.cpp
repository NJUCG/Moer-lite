#include <CoreLayer/Math/Math.h>
#include <ResourceLayer/Image.h>
#include <ResourceLayer/JsonUtil.h>
#include <fstream>
#include <stdio.h>

int main() {
  std::ifstream fstm("/home/zcx/Programming/Moer-lite/scenes/test.json");
  Json json = Json::parse(fstm);
  Vector3f size = fetchRequired<Vector3f>(json, "size");
  size.debugPrint();
}
