#include "UVIntegrator.h"
#include <ResourceLayer/Factory.h>

Spectrum UVIntegrator::li(Ray &ray, const Scene &scene,
                              std::shared_ptr<Sampler> sampler) const {
    auto intersectionOpt = scene.rayIntersect(ray);
    if (!intersectionOpt.has_value())
        return Spectrum(.0f);

    return Spectrum(intersectionOpt->texCoord.x(),intersectionOpt->texCoord.y(),0);
}
REGISTER_CLASS(UVIntegrator, "uv")
