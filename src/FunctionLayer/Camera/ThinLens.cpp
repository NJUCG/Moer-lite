#include "ThinLens.h"

ThinLensCamera::ThinLensCamera(const Json& json) : PerspectiveCamera(json) {
    lensRadius    = json["lensRadius"].get<float>();
    focalDistance = json["focalDistance"].get<float>();
}

Ray ThinLensCamera::sampleRay(const CameraSample& sample, Vector2f NDC) const {

    // TODO
}

Ray ThinLensCamera::sampleRayDifferentials(const CameraSample& sample, Vector2f NDC) const {
    // TODO
}

REGISTER_CLASS(ThinLensCamera, "thinlens")