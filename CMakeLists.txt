# 检查cmake的最低版本
cmake_minimum_required(VERSION 3.10.0)

# 设置在VS中建立文件夹
SET_PROPERTY( GLOBAL PROPERTY USE_FOLDERS ON )

# 定义工程名称
project(SPH_Fluid_Simulation)

set(GLFW_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/third/opengl/glfw)
set(GLFW_LIB ${CMAKE_SOURCE_DIR}/lib/glfw/libglfw3.a)
set(GLAD_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/third/opengl/GLAD)

#添加依赖的头文件
include_directories(
    ${GLFW_INCLUDE_DIR}
    ${GLAD_INCLUDE_DIR}
)

#收集文件
file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)
file(GLOB HEADERS ${CMAKE_SOURCE_DIR}/include/*.h)

#生成可执行程序
add_executable(${PROJECT_NAME}
    ${HEADERS}
    ${SOURCES}
    ${CMAKE_SOURCE_DIR}/src/glad.c
)

#此处添加依赖库
target_link_libraries(${PROJECT_NAME}
    ${GLFW_LIB}
)