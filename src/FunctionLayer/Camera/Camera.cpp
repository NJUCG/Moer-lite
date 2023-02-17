#include "Camera.h"

//* 根据json对象初始化相机的各个参数
Camera::Camera(const Json &json) {
  tNear = fetchOptional(json, "tNear", 1e-4f); // 如不提供默认为0.0001
  tFar = fetchOptional(json, "tFar", 1e10f);   // 如不提供默认为10^10
  timeStart = fetchOptional(json, "timeStart", .0f); // 如不提供默认为0
  timeEnd = fetchOptional(json, "timeEnd", .0f);     // 如不提供默认为0
  film = std::make_shared<Film>(json["film"]); // 根据相机的film域创建一个film
}

//* 根据json对象初始化透视相机的各个参数
PerspectiveCamera::PerspectiveCamera(const Json &json) : Camera(json) {
  //* 初始化透视相机必须提供position, lookAt以及up三个参数
  Point3f position = fetchRequired<Point3f>(json["transform"], "position"),
          lookAt = fetchRequired<Point3f>(json["transform"], "lookAt");
  Vector3f up = fetchRequired<Vector3f>(json["transform"], "up");
  //* 还需要提供fov
  verticalFov = fetchRequired<float>(json, "verticalFov") / 180 * PI,
  aspectRatio = (float)this->film->size[0] / this->film->size[1];

  //* 计算
  Vector3f forward = normalize(lookAt - position),
           right = normalize(cross(forward, up));
  up = normalize(cross(right, forward));

  Matrix4f translation =
      Transform::translation(Vector3f{position[0], position[1], position[2]});
  Matrix4f rotation;
  rotation.rows[0][0] = right[0];
  rotation.rows[1][0] = right[1];
  rotation.rows[2][0] = right[2];
  rotation.rows[3][0] = 0;

  rotation.rows[0][1] = up[0];
  rotation.rows[1][1] = up[1];
  rotation.rows[2][1] = up[2];
  rotation.rows[3][1] = 0;

  rotation.rows[0][2] = -forward[0];
  rotation.rows[1][2] = -forward[1];
  rotation.rows[2][2] = -forward[2];
  rotation.rows[3][2] = 0;

  rotation.rows[0][3] = 0;
  rotation.rows[1][3] = 0;
  rotation.rows[2][3] = 0;
  rotation.rows[3][3] = 1;

  transform = Transform(translation, rotation, Matrix4f::identity());
}
