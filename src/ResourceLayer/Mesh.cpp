#include "Mesh.h"
#include "FileUtil.h"
#include <map>
#include <tiny_obj_loader.h>

using namespace tinyobj;
std::shared_ptr<MeshData> MeshData::loadFromFile(std::string filepath) {
  filepath = FileUtil::getFullPath(filepath);

  //* 避免同一个Mesh文件被加载多次，每次加载前先进行查询
  static std::map<std::string, std::shared_ptr<MeshData>> meshPool{};

  if (meshPool.count(filepath) != 0)
    return meshPool[filepath];

  ObjReader reader;
  ObjReaderConfig config;
  config.triangulate = true; // 将所有mesh都三角型化

  if (!reader.ParseFromFile(filepath, config)) {
    if (!reader.Error().empty()) {
      std::cerr << "Error in parsing obj file " << filepath << " : "
                << reader.Error() << std::endl;
      exit(1);
    }
  }

  if (!reader.Warning().empty()) {
    std::cout << "Warning in parsing obj file " << filepath << " : "
              << reader.Warning() << std::endl;
  }

  const auto &attrib = reader.GetAttrib();
  const auto &shapes = reader.GetShapes();

  if (shapes.size() != 1) {
    std::cerr << "目前只支持每个.obj文件中包含一个Mesh\n";
    exit(1);
  }
  std::shared_ptr<MeshData> mesh = std::make_shared<MeshData>();
  const auto shape = shapes[0];

  size_t faceCount = shape.mesh.num_face_vertices.size(),
         vertexCount = attrib.vertices.size() / 3;

  mesh->faceCount = faceCount;
  mesh->vertexCount = vertexCount;
  mesh->vertexBuffer.reserve(vertexCount);
  mesh->faceBuffer.reserve(faceCount);

  for (int i = 0; i < faceCount; ++i) {
    std::array<DataIndex, 3> triangleInfo;
    for (int v = 0; v < 3; ++v) {
      DataIndex dataIndex;
      auto index = shape.mesh.indices[i * 3 + v];
      dataIndex.vertexIndex = index.vertex_index;
      dataIndex.normalIndex = index.normal_index;
      dataIndex.texcodIndex = index.texcoord_index;
      triangleInfo[v] = dataIndex;
    }
    mesh->faceBuffer.emplace_back(triangleInfo);
  }

  const auto &vertexBuffer = attrib.GetVertices();
  const auto &normalBuffer = attrib.normals;
  const auto &texcodBuffer = attrib.texcoords;
  for (int i = 0; i < vertexCount; ++i) {
    float vx = vertexBuffer[i * 3], vy = vertexBuffer[i * 3 + 1],
          vz = vertexBuffer[i * 3 + 2];
    mesh->vertexBuffer.emplace_back(Point3f{vx, vy, vz});
  }

  //* 每个obj文件一定会包含Mesh的vertexBuffer和faceBuffer
  //* 但是normalBuffer和texcoordBuffer是可选的
  if (normalBuffer.size() != 0) {
    int normalBufferSize = normalBuffer.size() / 3;
    mesh->normalBuffer.reserve(normalBufferSize);
    for (int i = 0; i < normalBufferSize; ++i) {
      float nx = normalBuffer[i * 3], ny = normalBuffer[i * 3 + 1],
            nz = normalBuffer[i * 3 + 2];
      mesh->normalBuffer.emplace_back(Vector3f{nx, ny, nz});
    }
  }
  if (texcodBuffer.size() != 0) {
    int texcodBufferSize = texcodBuffer.size() / 2;
    mesh->texcodBuffer.reserve(texcodBufferSize);
    for (int i = 0; i < texcodBufferSize; ++i) {
      float u = texcodBuffer[i * 2], v = texcodBuffer[i * 2 + 1];
      mesh->texcodBuffer.emplace_back(Vector2f{u, v});
    }
  }

  return mesh;
}