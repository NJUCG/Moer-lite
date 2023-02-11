#include <CoreLayer/Math/Math.h>
#include <FunctionLayer/Camera/Pinhole.h>
#include <FunctionLayer/Integrator/Integrator.h>
#include <FunctionLayer/Sampler/Sampler.h>
#include <FunctionLayer/Scene/Scene.h>
#include <ResourceLayer/Factory.h>
#include <ResourceLayer/FileUtil.h>
#include <ResourceLayer/Image.h>
#include <ResourceLayer/JsonUtil.h>
#include <fstream>
#include <stdio.h>
int main(int argc, char **argv) {
  const char *sceneDir = argv[1];
  FileUtil::setWorkingDirectory(sceneDir);
  std::string sceneJsonPath = FileUtil::getFullPath("scene.json");
  std::ifstream fstm(sceneJsonPath);
  Json json = Json::parse(fstm);

  const auto &camera_type = fetchRequired<std::string>(json["camera"], "type");
  const auto &integrator_type =
      fetchRequired<std::string>(json["integrator"], "type");
  const auto &sampler_type =
      fetchRequired<std::string>(json["sampler"], "type");

  auto camera = Factory::construct_class<Camera>(camera_type, json["camera"]);
  auto scene = std::make_shared<Scene>(json["scene"]);
  auto integrator =
      Factory::construct_class<Integrator>(integrator_type, json["integrator"]);
  auto sampler =
      Factory::construct_class<Sampler>(sampler_type, json["sampler"]);
  int spp = sampler->xSamples * sampler->ySamples;
  int width = camera->film->size[0], height = camera->film->size[1];
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      Vector2f NDC{(float)x / width, (float)y / height};
      Spectrum li(.0f);
      for (int i = 0; i < spp; ++i) {
        Ray ray = camera->sampleRay(CameraSample(), NDC);
        li += integrator->li(ray, *scene, sampler);
      }
      camera->film->deposit({x, y}, li / spp);
    }
  }
  camera->film->savePNG(
      fetchRequired<std::string>(json["output"], "filename").c_str());
}
