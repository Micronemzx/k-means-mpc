# 获取 Google Test
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.16.0 # 可以根据需要更改版本
)
# 使 Google Test 内容可用
FetchContent_MakeAvailable(googletest)

# 包含项目根目录的头文件
include_directories(../)
include_directories(../deps/boost_1_82_0)

# 添加可执行文件，这里假设测试文件为 pailler_test.cpp
add_executable(k_means_mpc_test pailler_test.cpp ../pailler.cpp)

# 链接 Google Test 库
# link_directories(../deps/boost_1_82_0/stage/lib)
target_link_libraries(k_means_mpc_test gtest_main oc::cryptoTools ntl gmp)

add_executable(nlt_test bytesfromzz.cpp)
target_link_libraries(nlt_test ntl gmp m)

add_executable(sdiv_test sdiv_test.cpp)
target_link_libraries(sdiv_test ntl gmp m mpc)

add_executable(scomp_test scomp_test.cpp)
target_link_libraries(scomp_test ntl gmp m mpc)

add_executable(smink_test smink_test.cpp)
target_link_libraries(smink_test ntl gmp m mpc)

add_executable(async_test async_send_test.cpp)
target_link_libraries(async_test ntl gmp m mpc)