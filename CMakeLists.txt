cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

project(Moer-lite LANGUAGES CXX)

add_executable(Moer 

src/main.cpp 

src/CoreLayer/Math/Transform.cpp

src/ResourceLayer/Image.cpp
src/ResourceLayer/JsonUtil.cpp

)
target_include_directories(Moer PUBLIC ./externals)
target_include_directories(Moer PUBLIC ./src)
target_include_directories(Moer PUBLIC ./externals/json/include)
 