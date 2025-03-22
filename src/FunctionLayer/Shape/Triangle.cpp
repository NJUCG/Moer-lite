#include "Triangle.h"
#include "CoreLayer/Math/Constant.h"
#include "CoreLayer/Math/Geometry.h"
#include <FunctionLayer/Acceleration/Linear.h>
#include <cstdlib>
//--- Triangle ---
Triangle::Triangle(int _primID, int _vtx0Idx, int _vtx1Idx, int _vtx2Idx,
                   const TriangleMesh *_mesh)
    : primID(_primID), vtx0Idx(_vtx0Idx), vtx1Idx(_vtx1Idx), vtx2Idx(_vtx2Idx),
      mesh(_mesh) {
  Point3f vtx0 = mesh->transform.toWorld(mesh->meshData->vertexBuffer[vtx0Idx]),
          vtx1 = mesh->transform.toWorld(mesh->meshData->vertexBuffer[vtx1Idx]),
          vtx2 = mesh->transform.toWorld(mesh->meshData->vertexBuffer[vtx2Idx]);
  boundingBox.Expand(vtx0);
  boundingBox.Expand(vtx1);
  boundingBox.Expand(vtx2);
  this->geometryID = mesh->geometryID;
}

bool Triangle::rayIntersectShape(Ray &ray, int *primID, float *u,
                                 float *v) const {
  // We use the Möller-Trumbore algorithm to solve the intersection
  // of a triangle and a ray

  // Reference: https://chatgpt.com/share/67dd92e0-ad98-800f-801f-249ea0c995c6

  // Step 1: Setup
  auto V0 = mesh->transform.toWorld(mesh->meshData->vertexBuffer[vtx0Idx]),
       V1 = mesh->transform.toWorld(mesh->meshData->vertexBuffer[vtx1Idx]),
       V2 = mesh->transform.toWorld(mesh->meshData->vertexBuffer[vtx2Idx]);
  auto O = ray.origin;
  auto D = ray.direction;

  // Step 2: Calculate Edge Vectors
  auto E1 = V1 - V0, E2 = V2 - V0;

  // Step 3: Compute the Determinant
  auto P = cross(D, E2);
  auto det = dot(E1, P);

  // If it's close to 0, then ray is parallel to surface, no intersection
  if (abs(det) < EPSILON)
    return false;

  // Step 4: Calculate Inverse Determinant
  auto inv_det = 1.0f / det;

  // Step 5: Calculate the U Parameter
  auto T = O - V0;
  auto u_intersect = inv_det * dot(T, P);
  if (u_intersect < 0 || u_intersect > 1) // Outside triangle
    return false;

  // Step 6: Calculate the V Parameter
  auto Q = cross(T, E1);
  auto v_intersect = inv_det * dot(D, Q);
  if (v_intersect < 0 || u_intersect + v_intersect > 1) // Outside triangle
    return false;

  // Step 7: Calculate the Distance Parameter
  auto t_intersect = inv_det * dot(E2, Q);
  if (t_intersect < 0) // Intersects in the reverse direction
    return false;

  // Output results
  *primID = this->primID;
  *u = u_intersect;
  *v = v_intersect;

  return true;
}

void Triangle::fillIntersection(float distance, int primID, float u, float v,
                                Intersection *intersection) const {
  // 该函数实际上不会被调用
  return;
}

//--- TriangleMesh ---
TriangleMesh::TriangleMesh(const Json &json) : Shape(json) {
  const auto &filepath = fetchRequired<std::string>(json, "file");
  meshData = MeshData::loadFromFile(filepath);
}

RTCGeometry TriangleMesh::getEmbreeGeometry(RTCDevice device) const {
  RTCGeometry geometry = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);

  float *vertexBuffer = (float *)rtcSetNewGeometryBuffer(
      geometry, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, 3 * sizeof(float),
      meshData->vertexCount);
  for (int i = 0; i < meshData->vertexCount; ++i) {
    Point3f vertex = transform.toWorld(meshData->vertexBuffer[i]);
    vertexBuffer[3 * i] = vertex[0];
    vertexBuffer[3 * i + 1] = vertex[1];
    vertexBuffer[3 * i + 2] = vertex[2];
  }

  unsigned *indexBuffer = (unsigned *)rtcSetNewGeometryBuffer(
      geometry, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3,
      3 * sizeof(unsigned), meshData->faceCount);
  for (int i = 0; i < meshData->faceCount; ++i) {
    indexBuffer[i * 3] = meshData->faceBuffer[i][0].vertexIndex;
    indexBuffer[i * 3 + 1] = meshData->faceBuffer[i][1].vertexIndex;
    indexBuffer[i * 3 + 2] = meshData->faceBuffer[i][2].vertexIndex;
  }
  rtcCommitGeometry(geometry);
  return geometry;
}

bool TriangleMesh::rayIntersectShape(Ray &ray, int *primID, float *u,
                                     float *v) const {
  //* 当使用embree加速时，该方法不会被调用
  int geomID = -1;
  return acceleration->rayIntersect(ray, &geomID, primID, u, v);
}

void TriangleMesh::fillIntersection(float distance, int primID, float u,
                                    float v, Intersection *intersection) const {
  //* todo 填充光线与三角网格求交得到的交点信息
  intersection->distance = distance;
  intersection->shape = this;
  //* 1. 在三角形内部用插值计算交点坐标
  //* 2. 在三角形内部用插值计算法线
  //* 3. 在三角形内部用插值计算纹理坐标
  //* 4. 在三角形内部用插值计算交点的切线和副切线

  auto [didx0, didx1, didx2] = this->meshData->faceBuffer[primID];
  auto [vidx0, vidx1, vidx2] =
      std::tie(didx0.vertexIndex, didx1.vertexIndex, didx2.vertexIndex);
  auto [v0, v1, v2] = std::tie(this->meshData->vertexBuffer[vidx0],
                               this->meshData->vertexBuffer[vidx1],
                               this->meshData->vertexBuffer[vidx2]);
}

void TriangleMesh::initInternalAcceleration() {
  acceleration = Acceleration::createAcceleration();
  int primCount = meshData->faceCount;
  for (int primID = 0; primID < primCount; ++primID) {
    int vtx0Idx = meshData->faceBuffer[primID][0].vertexIndex,
        vtx1Idx = meshData->faceBuffer[primID][1].vertexIndex,
        vtx2Idx = meshData->faceBuffer[primID][2].vertexIndex;
    std::shared_ptr<Triangle> triangle =
        std::make_shared<Triangle>(primID, vtx0Idx, vtx1Idx, vtx2Idx, this);
    acceleration->attachShape(triangle);
  }
  acceleration->build();
  // TriangleMesh的包围盒就是其内部加速结构的包围盒
  boundingBox = acceleration->boundingBox;
}
REGISTER_CLASS(TriangleMesh, "triangle")
