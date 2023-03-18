#include "Acceleration.h"
#include "EmbreeBVH.h"
#include "Linear.h"
#include "Octree.h"
#include "BVH.h"
// Default acceleration type is embree
AccelerationType Acceleration::type = AccelerationType::Embree;

std::map<std::string, AccelerationType> accelerationTypeMap = {
    {"embree", AccelerationType::Embree},
    {"linear", AccelerationType::Linear},
    {"octree", AccelerationType::Octree},
    {"bvh",AccelerationType::BVH}};

std::map<AccelerationType, std::function<std::shared_ptr<Acceleration>()>>
    accelerationBuildMap = {
        {AccelerationType::Embree, std::make_shared<EmbreeBVH>},
        {AccelerationType::Linear, std::make_shared<LinearAcceleration>},
        {AccelerationType::Octree, std::make_shared<Octree>},
        {AccelerationType::BVH, std::make_shared<BVH>}
};

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
