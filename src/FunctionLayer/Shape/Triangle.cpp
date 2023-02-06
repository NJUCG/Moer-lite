#include "Triangle.h"

Triangle::Triangle(const Transform &_transform, std::shared_ptr<Mesh> _mesh)
    : Shape(_transform), mesh(_mesh) {}

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