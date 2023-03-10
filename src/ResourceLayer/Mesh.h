#pragma once
#include <CoreLayer/Math/Math.h>
#include <array>
#include <memory>
#include <vector>
//* 目前只支持三角形Mesh

struct DataIndex {
  int vertexIndex; // 顶点的索引
  int normalIndex; // 法线的索引
  int texcodIndex; // 纹理坐标的索引
};

struct MeshData {
public:
  int faceCount;                      // 该Mesh三角形面的个数
  int vertexCount;                    // 该Mesh顶点的个数
  std::vector<Point3f> vertexBuffer;  // 存储该Mesh的所有顶点
  std::vector<Vector3f> normalBuffer; // 存储该Mesh的所有法线
  std::vector<Vector2f> texcodBuffer; // 存储该Mesh每个顶点的纹理坐标
  std::vector<std::array<DataIndex, 3>>
      faceBuffer; // 存储该Mesh每个面（即三角形）的三个顶点对应数据的索引

  //* 将一个.obj文件加载到内存中
  //* 请确保加载的.obj文件中只包含一个Mesh
  static std::shared_ptr<MeshData> loadFromFile(std::string filepath);
};