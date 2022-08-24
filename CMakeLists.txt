cmake_minimum_required(VERSION 2.8)

project(CopyFile)

find_package(Boost 1.74.0)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(CopyFile)
add_subdirectory(CopyFileLib)
add_subdirectory(GoogleTests)

