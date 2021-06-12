cmake_minimum_required(VERSION 3.10)

project(recover)

set(SRC 
${PROJECT_SOURCE_DIR}/test.c
${PROJECT_SOURCE_DIR}/../../interface.c
${PROJECT_SOURCE_DIR}/../../../buffer/buffer.c
${PROJECT_SOURCE_DIR}/../../../queue/queue.c
${PROJECT_SOURCE_DIR}/../../../index/index.c
${PROJECT_SOURCE_DIR}/../../../persis/persis.c
${PROJECT_SOURCE_DIR}/../../../sync/sync.c
)

SET(CMAKE_BUILD_TYPE "Debug")  # 定义编译类型
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb") # 定义Debug编译参数
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall") # 定义Release编译参数

add_executable(${PROJECT_NAME} ${SRC})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}  pthread)