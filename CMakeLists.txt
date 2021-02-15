cmake_minimum_required(VERSION 2.8)
project(test)


set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

set(CMAKE_C_FLAGS "-lpthread -lz -g")


#添加子目录
add_subdirectory(Net)
add_subdirectory(time)
add_subdirectory(Util)

#       Net ——Kanet
#       time  ——Katime
#       Util ——Kautil


# 指定生成目标 
add_executable(test main.cc)


# 添加链接库
target_link_libraries(test KaNet Katime Kautil pthread protobuf)

