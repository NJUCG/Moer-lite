#include "Cube.h"
#include "ResourceLayer/Factory.h"

// Cube的transform有特殊处理

Cube::Cube(const Json &json) : Shape(json) {
  // Cube默认是中心位于(0, 0, 0)处，棱长为2的AABB，其形状可以通过transform调整
  boxMin = Point3f{-1.f, -1.f, -1.f};
  boxMax = Point3f{1.f, 1.f, 1.f};

  // 构建AABB
  for (int i = 0; i < 8; ++i) {
    Point3f p;
    p[0] = (i & 0b100) ? boxMax[0] : boxMin[0];
    p[1] = (i & 0b010) ? boxMax[1] : boxMin[1];
    p[2] = (i & 0b001) ? boxMax[2] : boxMin[2];
    p = transform.toWorld(p);

    for (int j = 0; j < 3; ++j) {
      boundingBox.Expand(p);
    }
  }

  // 在计算时，所有计算都是在局部坐标系内完成的，因此这里只对boxMin和boxMax做scale操作
  Matrix4f scale = transform.scale;
  vecmat::vec4f min{boxMin[0], boxMin[1], boxMin[2], 1.f},
      max{boxMax[0], boxMax[1], boxMax[2], 1.f};
  min = scale * min, max = scale * max;
  min /= min[3], max /= max[3];
  boxMin = Point3f{min[0], min[1], min[2]};
  boxMax = Point3f{max[0], max[1], max[2]};
}

bool Cube::rayIntersectShape(Ray &ray, int *primID, float *u, float *v) const {
  // 我们将shape的旋转和平移的逆变换应用到光线上，在不改变两者的相对位置的情况下
  // 在局部坐标系中完成求交计算，局部坐标系中cube一直是Axis-aligned box
  Point3f origin = ray.origin;
  Vector3f direction = ray.direction;

  vecmat::vec4f o{origin[0], origin[1], origin[2], 1.f},
      d{direction[0], direction[1], direction[2], 0.f};

  o = transform.invRotate * transform.invTranslate * o;
  d = transform.invRotate * transform.invTranslate * d;

  o /= o[3];
  origin = Point3f{o[0], o[1], o[2]};
  direction = Vector3f{d[0], d[1], d[2]};

  float tNear = ray.tNear, tFar = ray.tFar;
  for (int i = 0; i < 3; ++i) {
    float invDir = 1.f / direction[i]; // 没有做除0检查，该算法貌似不需要？
    float t0 = (boxMin[i] - origin[i]) * invDir,
          t1 = (boxMax[i] - origin[i]) * invDir;
    if (t0 > t1)
      std::swap(t0, t1);
    tNear = std::max(tNear, t0);
    tFar = std::min(tFar, t1);

    if (tNear > tFar)
      return false;
  }

  auto compute = [min = boxMin, max = boxMax](Point3f hitpoint, int *primID,
                                              float *u, float *v) {
    float minBias = FLT_MAX;
    int flag = -1;
    for (int i = 0; i < 3; ++i) {
      if (float bias = std::abs(hitpoint[i] - min[i]); bias < minBias) {
        flag = 2 * i;
        minBias = bias;
      }
      if (float bias = std::abs(hitpoint[i] - max[i]); bias < minBias) {
        flag = 2 * i + 1;
        minBias = bias;
      }
    }

    *primID = flag;
    int axis = (flag / 2 + 1) % 3;
    *u = (float)(hitpoint[axis] - min[axis]) / (max[axis] - min[axis]);
    axis = (axis + 1) % 3;
    *v = (float)(hitpoint[axis] - min[axis]) / (max[axis] - min[axis]);
  };

  bool hit = false;
  if (ray.tNear < tFar && tFar < ray.tFar) {
    Point3f hitpoint = origin + tFar * direction;
    compute(hitpoint, primID, u, v);
    ray.tFar = tFar;
    hit = true;
  }
  if (ray.tNear < tNear && tNear < ray.tFar) {
    Point3f hitpoint = origin + tNear * direction;
    compute(hitpoint, primID, u, v);
    ray.tFar = tNear;
    hit = true;
  }
  return hit;
}

void Cube::fillIntersection(float distance, int primID, float u, float v,
                            Intersection *intersection) const {
  intersection->shape = this;
  intersection->distance = distance;
  vecmat::vec4f normal{.0f, .0f, .0f, .0f};
  normal[primID / 2] = (primID % 2) ? 1 : -1;

  normal = transform.rotate * normal;
  intersection->normal = normalize(Vector3f{normal[0], normal[1], normal[2]});

  vecmat::vec4f hitpoint;
  hitpoint[primID / 2] = (primID % 2) ? boxMax[primID / 2] : boxMin[primID / 2];
  int axis = (primID / 2 + 1) % 3;
  hitpoint[axis] = boxMin[axis] + u * (boxMax[axis] - boxMin[axis]);
  axis = (axis + 1) % 3;
  hitpoint[axis] = boxMin[axis] + v * (boxMax[axis] - boxMin[axis]);
  hitpoint[3] = 1.f;

  hitpoint = transform.translate * transform.rotate * hitpoint;
  hitpoint /= hitpoint[3];

  intersection->position = Point3f{hitpoint[0], hitpoint[1], hitpoint[2]};

  intersection->texCoord = Vector2f{u, v};
  // TODO 计算交点的切线和副切线
  Vector3f tangent{1.f, 0.f, .0f};
  Vector3f bitangent;
  if (std::abs(dot(tangent, intersection->normal)) > .9f) {
    tangent = Vector3f(.0f, 1.f, .0f);
  }
  bitangent = normalize(cross(tangent, intersection->normal));
  tangent = normalize(cross(intersection->normal, bitangent));
  intersection->tangent = tangent;
  intersection->bitangent = bitangent;
}

REGISTER_CLASS(Cube, "cube")