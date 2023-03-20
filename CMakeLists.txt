cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(Moer-lite LANGUAGES CXX)

if(MSVC)
  add_compile_options("/MP")
  add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
  add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
  add_definitions(-D_USE_MATH_DEFINES)
endif(MSVC)

if(WIN32)
  add_definitions(-DUNICODE -D_UNICODE)
  add_definitions(-DNOMINMAX)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/target/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/target/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/target/bin)

add_subdirectory(./externals)

add_executable(Moer
  src/main.cpp

  src/CoreLayer/Math/Transform.cpp

  src/FunctionLayer/Acceleration/AABB.cpp
  src/FunctionLayer/Acceleration/Acceleration.cpp
  src/FunctionLayer/Acceleration/EmbreeBVH.cpp
  src/FunctionLayer/Acceleration/Linear.cpp
  src/FunctionLayer/Acceleration/Octree.cpp

  src/FunctionLayer/Integrator/NormalIntegrator.cpp
  src/FunctionLayer/Integrator/DirectIntegrator.cpp
  src/FunctionLayer/Integrator/WhittedIntegrator.cpp
  src/FunctionLayer/Camera/Camera.cpp
  src/FunctionLayer/Camera/Pinhole.cpp
  src/FunctionLayer/Scene/Scene.cpp
  src/FunctionLayer/Light/SpotLight.cpp
  src/FunctionLayer/Light/AreaLight.cpp
  src/FunctionLayer/Light/EnvironmentLight.cpp
  src/FunctionLayer/Shape/Shape.cpp
  src/FunctionLayer/Shape/Triangle.cpp
  src/FunctionLayer/Shape/Cube.cpp
  src/FunctionLayer/Shape/Sphere.cpp
  src/FunctionLayer/Shape/Parallelogram.cpp
  src/FunctionLayer/Texture/NormalTexture.cpp
  src/FunctionLayer/Texture/ImageTexture.cpp
  src/FunctionLayer/Texture/Texture.cpp
  src/FunctionLayer/Texture/Mipmap.cpp
  src/FunctionLayer/Material/Matte.cpp
  src/FunctionLayer/Material/Mirror.cpp
  src/FunctionLayer/Material/Material.cpp
  src/FunctionLayer/Sampler/IndependentSampler.cpp
  src/ResourceLayer/Image.cpp
  src/ResourceLayer/Mesh.cpp
  src/ResourceLayer/FileUtil.cpp
        src/FunctionLayer/Shape/Disk.cpp src/FunctionLayer/Shape/Disk.h src/FunctionLayer/Shape/Cone.cpp src/FunctionLayer/Shape/Cone.h src/FunctionLayer/Shape/Cylinder.cpp src/FunctionLayer/Shape/Cylinder.h src/FunctionLayer/Integrator/UVIntegrator.cpp src/FunctionLayer/Integrator/UVIntegrator.h src/FunctionLayer/Acceleration/BVH.cpp src/FunctionLayer/Acceleration/BVH.h)

target_include_directories(Moer PUBLIC ./externals)
target_include_directories(Moer PUBLIC ./src)
target_include_directories(Moer PUBLIC ./externals/json/include)
target_include_directories(Moer PUBLIC ./externals/embree/include)

if(WIN32)
  set(LIBS_PATH ${PROJECT_SOURCE_DIR}/libs/win)
  set(EMBREE_DLL ${LIBS_PATH}/embree3.dll ${LIBS_PATH}/tbb12.dll)
endif()

if(UNIX AND NOT APPLE)
  set(LIBS_PATH ${PROJECT_SOURCE_DIR}/libs/linux)
  set(EMBREE_DLL ${LIBS_PATH}/libembree3.so.3 ${LIBS_PATH}/libtbb.so.12.2)
endif()

if(APPLE)
  set(LIBS_PATH ${PROJECT_SOURCE_DIR}/libs/macos)
  set(EMBREE_DLL ${LIBS_PATH}/libembree3.3.dylib ${LIBS_PATH}/libtbb.12.2.dylib)
endif()


target_link_directories(Moer PUBLIC ${LIBS_PATH})
target_link_directories(Moer PUBLIC ${PROJECT_SOURCE_DIR}/target/lib)

add_custom_command(TARGET Moer POST_BUILD
  DEPENDS ${LIBS_PATH}/embree3.dll
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
  ${EMBREE_DLL} $<TARGET_FILE_DIR:Moer>)

target_link_libraries(Moer PUBLIC embree3)
target_link_libraries(Moer PUBLIC tinyobjloader)

if(APPLE)
  execute_process(
    COMMAND uname -m
    RESULT_VARIABLE result
    OUTPUT_VARIABLE MOER_OSX_NATIVE_ARCHITECTURE
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  if(${MOER_OSX_NATIVE_ARCHITECTURE} STREQUAL "arm64")
    set_target_properties(Moer PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
  endif()
endif()