#include "Triangle.h"

Triangle::Triangle(const Json &json) : Shape(json) {
  const auto &filepath = fetchRequired<std::string>(json, "file");
  mesh = Mesh::loadFromFile(filepath);
}

RTCGeometry Triangle::getEmbreeGeometry(RTCDevice device) const {
  RTCGeometry geometry = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);

  float *vertexBuffer = (float *)rtcSetNewGeometryBuffer(
      geometry, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, 3 * sizeof(float),
      mesh->vertexCount);
  for (int i = 0; i < mesh->vertexCount; ++i) {
    Point3f vertex = transform.toWorld(mesh->vertexBuffer[i]);
    vertexBuffer[3 * i] = vertex[0];
    vertexBuffer[3 * i + 1] = vertex[1];
    vertexBuffer[3 * i + 2] = vertex[2];
  }

  unsigned *indexBuffer = (unsigned *)rtcSetNewGeometryBuffer(
      geometry, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3,
      3 * sizeof(unsigned), mesh->faceCount);
  for (int i = 0; i < mesh->faceCount; ++i) {
    indexBuffer[i * 3] = mesh->faceBuffer[i][0].vertexIndex;
    indexBuffer[i * 3 + 1] = mesh->faceBuffer[i][1].vertexIndex;
    indexBuffer[i * 3 + 2] = mesh->faceBuffer[i][2].vertexIndex;
  }
  rtcCommitGeometry(geometry);
  return geometry;
}

bool Triangle::rayIntersectShape(const Ray &ray, float *distance, int *primID,
                                 float *u, float *v) const {
  //* 当使用embree加速时，该方法不会被调用
  // TODO 自行实现加速结构时请实现该方法
  return false;
}

void Triangle::fillIntersection(float distance, int primID, float u, float v,
                                Intersection *intersection) const {
  intersection->distance = distance;
  intersection->shape = this;
  //* 在三角形内部用插值计算交点、法线以及纹理坐标
  auto faceInfo = mesh->faceBuffer[primID];
  float w = 1.f - u - v;

  //* 计算交点
  Point3f pw = transform.toWorld(mesh->vertexBuffer[faceInfo[0].vertexIndex]),
          pu = transform.toWorld(mesh->vertexBuffer[faceInfo[1].vertexIndex]),
          pv = transform.toWorld(mesh->vertexBuffer[faceInfo[2].vertexIndex]);
  intersection->position = Point3f{w * pw[0] + u * pu[0] + v * pv[0],
                                   w * pw[1] + u * pu[1] + v * pv[1],
                                   w * pw[2] + u * pu[2] + v * pv[2]};
  //* 计算法线
  if (mesh->normalBuffer.size() != 0) {
    Vector3f nw =
                 transform.toWorld(mesh->normalBuffer[faceInfo[0].normalIndex]),
             nu =
                 transform.toWorld(mesh->normalBuffer[faceInfo[1].normalIndex]),
             nv =
                 transform.toWorld(mesh->normalBuffer[faceInfo[2].normalIndex]);
    intersection->normal = normalize(w * nw + u * nu + v * nv);
  } else {
    intersection->normal = normalize(cross(pu - pw, pv - pw));
  }

  //* 计算纹理坐标
  if (mesh->texcodBuffer.size() != 0) {
    Vector2f tw = mesh->texcodBuffer[faceInfo[0].texcodIndex],
             tu = mesh->texcodBuffer[faceInfo[1].texcodIndex],
             tv = mesh->texcodBuffer[faceInfo[2].texcodIndex];
    intersection->texCoord = w * tw + u * tu + v * tv;
  } else {
    intersection->texCoord = Vector2f{.0f, .0f};
  }

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
REGISTER_CLASS(Triangle, "triangle")