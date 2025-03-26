# 设置CMake的最低版本要求
cmake_minimum_required(VERSION 3.10)

# 设置项目名称
project(k-means-mpc)
# 设置C++标准
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# 包含 FetchContent 模块
include(FetchContent)
# 获取 yaml-cpp
FetchContent_Declare(
    yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG master # 可以根据需要更改版本
)
# 使 Google Test 内容可用
FetchContent_MakeAvailable(yaml-cpp)

include_directories(./deps/boost_1_82_0)

add_library(mpc STATIC pailler.cpp secure_proto.cpp)

link_directories(./deps/boost_1_82_0/stage/lib)
target_link_libraries(mpc ntl gmp m libboost_system.a libboost_timer.a libboost_chrono.a)

# 添加可执行文件
add_executable(k-means-mpc main.cpp config.cpp server1.cpp)

# 链接GMP和NTL库
target_link_libraries(k-means-mpc yaml-cpp mpc)

# 添加测试子目录
add_subdirectory(./tests)
