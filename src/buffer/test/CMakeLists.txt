cmake_minimum_required(VERSION 3.10)

project(buffer)

set(SRC 
${PROJECT_SOURCE_DIR}/../buffer.c
${PROJECT_SOURCE_DIR}/test.c
${PROJECT_SOURCE_DIR}/../../sync/sync.c
)

SET(CMAKE_BUILD_TYPE "Debug")  # 定义编译类型
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb") # 定义Debug编译参数
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall") # 定义Release编译参数

add_executable(${PROJECT_NAME} ${SRC})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}  pthread)

