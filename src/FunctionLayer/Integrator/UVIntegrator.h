#include "Integrator.h"
class UVIntegrator : public Integrator{
public:
    UVIntegrator(const Json & json){};
    Spectrum li(Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler) const override;
};