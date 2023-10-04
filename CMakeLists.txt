cmake_minimum_required(VERSION 3.13.4)

project(asd-lab1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS -m64)
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast")

set(CMAKE_BUILD_TYPE Release)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

#project

file(GLOB_RECURSE MY_PROJECT_SRC src/*.cpp)

add_executable(asd-lab1 ${MY_PROJECT_SRC}) 

# libs

link_directories(/usr/lib)

