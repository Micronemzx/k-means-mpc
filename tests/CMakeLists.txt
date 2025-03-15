# 包含 FetchContent 模块
include(FetchContent)

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

# 添加可执行文件，这里假设测试文件为 pailler_test.cpp
add_executable(k_means_mpc_test pailler_test.cpp ../pailler.cpp)

# 链接 Google Test 库
target_link_libraries(k_means_mpc_test gtest_main ntl gmp m)


# 定义测试
# enable_testing()
# add_test(NAME k-means-mpc-test COMMAND k-means-mpc-test)
