cmake_minimum_required(VERSION 3.10)

project(threadSyncC)

add_executable(app threadSyncC.c)
target_link_libraries(app pthread)

