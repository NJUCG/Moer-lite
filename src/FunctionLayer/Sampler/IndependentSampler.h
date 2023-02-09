#include "Sampler.h"
#include <ResourceLayer/JsonUtil.h>
#include <random>

class IndependentSampler : public Sampler {
public:
  IndependentSampler(const Json &json);

  virtual float next1D();

  virtual Vector2f next2D();

private:
  std::random_device rd;
  std::mt19937 gen;
  std::uniform_real_distribution<float> dist;
};