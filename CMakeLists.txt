cmake_minimum_required(VERSION 3.10)

project(EdithDB)

add_subdirectory(src/buffer/test)
add_subdirectory(src/index/test)
add_subdirectory(src/interface/test/interface)
add_subdirectory(src/interface/test/recover)
add_subdirectory(src/persis/test)
add_subdirectory(src/queue/test)
add_subdirectory(src/sync/test)