#include "EmbreeBVH.h"

EmbreeBVH::EmbreeBVH() : Acceleration() {
  //* 初始化embree
  device = rtcNewDevice(nullptr);
  scene = rtcNewScene(device);
}

void EmbreeBVH::build() {
  //* 遍历场景内的每个几何体，获取其embree描述
  for (const auto shape : shapes) {
    RTCGeometry geometry = shape->getEmbreeGeometry(device);
    //* 添加至embree中
    rtcAttachGeometry(scene, geometry);
    rtcReleaseGeometry(geometry);
  }
  //* 提交当前场景，构建加速结构
  rtcCommitScene(scene);
}

std::optional<Intersection> EmbreeBVH::rayIntersect(Ray &ray) const {
  RTCIntersectContext context;
  RTCRayHit rtcRayHit;
  rtcInitIntersectContext(&context);
  //* 初始化RTCRayHit
  rtcRayHit.ray.org_x = ray.origin[0];
  rtcRayHit.ray.org_y = ray.origin[1];
  rtcRayHit.ray.org_z = ray.origin[2];
  rtcRayHit.ray.dir_x = ray.direction[0];
  rtcRayHit.ray.dir_y = ray.direction[1];
  rtcRayHit.ray.dir_z = ray.direction[2];
  rtcRayHit.ray.tnear = ray.tNear;
  rtcRayHit.ray.tfar = ray.tFar;
  rtcRayHit.ray.mask = -1;
  rtcRayHit.ray.flags = 0;
  rtcRayHit.hit.geomID = RTC_INVALID_GEOMETRY_ID;

  //* 求交
  rtcIntersect1(scene, &context, &rtcRayHit);

  //* 如果光线与加速结构没有交点
  if (rtcRayHit.hit.geomID == RTC_INVALID_GEOMETRY_ID)
    return std::nullopt;

  //* 有交点，填充intersection数据结构
  ray.tFar = rtcRayHit.ray.tfar;
  auto shape = shapes[rtcRayHit.hit.geomID];
  Intersection intersection;
  shape->fillIntersection(rtcRayHit.ray.tfar, rtcRayHit.hit.primID,
                          rtcRayHit.hit.u, rtcRayHit.hit.v, &intersection);
  return std::make_optional(intersection);
}