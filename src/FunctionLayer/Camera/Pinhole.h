#pragma once

#include "Camera.h"
class PinholeCamera : public PerspectiveCamera {
public:
  PinholeCamera() = delete;

  PinholeCamera(const Json &json);

  virtual Ray sampleRay(const CameraSample &sample,
                        Vector2f NDC) const override;

  virtual Ray sampleRayDifferentials(const CameraSample &sample,
                                     Vector2f NDC) const override;
};
