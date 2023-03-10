#include "Acceleration.h"
#include "EmbreeBVH.h"
#include "Linear.h"

// Default acceleration type is embree
AccelerationType Acceleration::type = AccelerationType::Embree;

std::map<std::string, AccelerationType> accelerationTypeMap = {
    {"embree", AccelerationType::Embree}, {"linear", AccelerationType::Linear}};

std::map<AccelerationType, std::function<std::shared_ptr<Acceleration>()>>
    accelerationBuildMap = {
        {AccelerationType::Embree, std::make_shared<EmbreeBVH>},
        {AccelerationType::Linear, std::make_shared<LinearAcceleration>}};

void Acceleration::setAccelerationType(std::string type) {
  if (accelerationTypeMap.count(type) == 0) {
    std::cerr << "Unkown acceleration type " << type << "!\n";
    exit(1);
  }
  std::cout << "Using acceleration type " << type << "\n";
  Acceleration::type = accelerationTypeMap[type];
}

std::shared_ptr<Acceleration> Acceleration::createAcceleration() {
  return accelerationBuildMap[Acceleration::type]();
}
