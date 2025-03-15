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

# 添加可执行文件
add_executable(k-means-mpc main.cpp pailler.cpp)

# # 查找GMP库
# find_library(GMP_LIBRARY gmp)
# if(NOT GMP_LIBRARY)
#     message(FATAL_ERROR "GMP library not found")
# endif()

# # 查找NTL库
# find_library(NTL_LIBRARY ntl)
# if(NOT NTL_LIBRARY)
#     message(FATAL_ERROR "NTL library not found")
# endif()

# 链接GMP和NTL库
target_link_libraries(k-means-mpc ntl gmp m)

# 添加测试子目录
add_subdirectory(./tests)
