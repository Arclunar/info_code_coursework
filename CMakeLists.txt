cmake_minimum_required(VERSION 2.8)
# cmake --version 查看 一般3.0即可

project(CourseWork)

# 设置编译模式为debug
set(CMAKE_BUILD_TYPE Debug)

# 可执行文件
add_executable(main main.cpp)
add_executable(main2 main2.cpp)
add_executable(main0 main0.cpp)

