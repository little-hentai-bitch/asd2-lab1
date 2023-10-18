cmake_minimum_required(VERSION 3.13.4)

project(asd-lab1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-m64 -g -pg")
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast")

set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

#project

set(Boost_USE_STATIC_LIBS ON)

find_package(Boost  COMPONENTS iostreams)

file(GLOB_RECURSE MY_PROJECT_SRC src/*.cpp)

add_executable(asd-lab1 ${MY_PROJECT_SRC}) 

# libs

target_link_libraries(asd-lab1 LINK_PUBLIC ${Boost_LIBRARIES})

link_directories(/usr/lib)

