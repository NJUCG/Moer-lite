#include <CoreLayer/Math/Math.h>
#include <ResourceLayer/Image.h>
#include <ResourceLayer/JsonUtil.h>
#include <fstream>
#include <stdio.h>

int main() {
  std::ifstream fstm("/home/zcx/Programming/Moer-lite/scenes/test.json");
  Json json = Json::parse(fstm);
  Vector2i v2i = fetchRequired<Vector2i>(json, "vec2i");
  //  v2i.debug_print();
  float f = fetchRequired<float>(json, "f");
  printf("%.3f\n", f);
}
