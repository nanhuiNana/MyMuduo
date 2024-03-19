cmake_minimum_required(VERSION 2.5)

# 定义工程名称
project(mymuduo)

# 设置动态库/静态库的路径，放在根目录的lib文件夹下
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# 增加-std=c++11
set(CMAKE_CXX_STANDARD 11)

# 设置调试信息
add_definitions("-Wall -g")

# GLOB：将指定目录下搜索到的满足条件的所有文件名生成一个列表，并将其存储到变量中
# CMAKE_CURRENT_SOURCE_DIR 宏表示当前访问的 CMakeLists.txt 文件所在的路径
# 搜索当前目录的src目录下所有的源文件，并存储到变量中
file(GLOB MAIN_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc)
# 搜索当前目录的include目录下所有的头文件，并存储到变量中
file(GLOB MAIN_HEAD ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

message(${MAIN_SRC})

# 编译生成动态库mymuduo
add_library(mymuduo SHARED ${MAIN_SRC} ${MAIN_HEAD})
