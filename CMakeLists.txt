cmake_minimum_required(VERSION 3.5...3.31)
project(MyFirstProject)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 创建可执行目标
add_executable(${PROJECT_NAME} src/main.cpp)

# 指定头文件搜索路径
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)

# 指定库文件搜索路径
target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/lib)

# 链接 raylib 及 Windows 系统级依赖库
target_link_libraries(${PROJECT_NAME} PRIVATE raylib opengl32 gdi32 winmm)