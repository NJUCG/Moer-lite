#include "IndependentSampler.h"
#include <ResourceLayer/Factory.h>
IndependentSampler::IndependentSampler(const Json &json) : Sampler(json) {
  gen = std::mt19937(rd());
  dist = std::uniform_real_distribution<float>(.0f, 1.f);
}
float IndependentSampler::next1D() { return dist(gen); }

Vector2f IndependentSampler::next2D() { return Vector2f{dist(gen), dist(gen)}; }

REGISTER_CLASS(IndependentSampler, "independent")