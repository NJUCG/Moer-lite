#pragma once
#include "Camera.h"
class ThinLensCamera : public PerspectiveCamera {

public:
    ThinLensCamera() = delete;

    ThinLensCamera(const Json& json);

    virtual Ray sampleRay(const CameraSample& sample, Vector2f NDC) const override;

    virtual Ray sampleRayDifferentials(const CameraSample& sample, Vector2f NDC) const override;

private:
    float lensRadius, focalDistance;
};
