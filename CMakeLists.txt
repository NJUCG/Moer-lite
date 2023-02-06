cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(Moer-lite LANGUAGES CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/target/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/target/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/target/bin)

add_subdirectory(./externals)

add_executable(Moer 
src/main.cpp 

src/CoreLayer/Math/Transform.cpp

src/FunctionLayer/Acceleration/EmbreeBVH.cpp
src/FunctionLayer/Integrator/NormalIntegrator.cpp
src/FunctionLayer/Camera/Camera.cpp
src/FunctionLayer/Camera/Pinhole.cpp
src/FunctionLayer/Scene/Scene.cpp
src/FunctionLayer/Shape/Triangle.cpp


src/ResourceLayer/Image.cpp
src/ResourceLayer/Mesh.cpp
)

target_include_directories(Moer PUBLIC ./externals)
target_include_directories(Moer PUBLIC ./src)
target_include_directories(Moer PUBLIC ./externals/json/include)
target_include_directories(Moer PUBLIC ./externals/embree/include)
 
target_link_directories(Moer PUBLIC ${PROJECT_SOURCE_DIR}libs/linux)
target_link_libraries(Moer tinyobjloader embree3)