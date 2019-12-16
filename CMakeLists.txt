cmake_minimum_required(VERSION 2.8.12)

project(pallette C CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

#设置C++编译选项
set(CXX_FLAGS
    -g
    -DCHECK_PTHREAD_RETURN_VALUE
    -D_FILE_OFFSET_BITS=64
    -Wall
    -Wextra
    -Werror
    -Wconversion
    -Wno-unused-parameter
    -Wold-style-cast
    -Woverloaded-virtual
    -Wpointer-arith
    -Wshadow
    -Wwrite-strings
    -march=native
    -rdynamic
)

#设置编译器
set(CMAKE_CXX_COMPILER "g++")
add_compile_options(-std=c++11)
#设置Debug模式选项
set(CMAKE_CXX_FLAGS_DEBUG "-O0")
#设置Release模式选项
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -finline-limit=1000 -DNDEBUG")
#可执行文件存放路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
#库文件存放路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

include_directories(
    ${PROJECT_SOURCE_DIR}
)

add_subdirectory(pallette)
