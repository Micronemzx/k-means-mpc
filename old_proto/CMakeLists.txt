
cmake_minimum_required(VERSION 3.10)
project(old_test)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

include(FetchContent)
# 获取 yaml-cpp
FetchContent_Declare(
    yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG master # 可以根据需要更改版本
)
# 使 Google Test 内容可用
FetchContent_MakeAvailable(yaml-cpp)

find_package(cryptoTools REQUIRED)
# include_directories(../deps/boost_1_82_0)
link_directories(../deps/boost_1_82_0/stage/lib)
add_library(mpc STATIC pailler.cpp old_proto.cpp)
target_link_libraries(mpc ntl gmp m libboost_timer.a libboost_chrono.a oc::cryptoTools)

add_executable(sdiv_test sdiv_test)
target_link_libraries(sdiv_test ntl gmp m mpc)

add_executable(scomp_test seccom_test.cpp)
target_link_libraries(scomp_test ntl gmp m mpc)

add_executable(smink_test smink_test.cpp)
target_link_libraries(smink_test ntl gmp m mpc)

# add_executable(k_means_test main.cpp server.cpp config.cpp)
# target_link_libraries(k_means_test ntl gmp m mpc yaml-cpp)