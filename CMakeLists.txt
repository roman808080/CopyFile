cmake_minimum_required(VERSION 3.15)

project(CopyFile)

find_package(Boost 1.79.0)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

find_package(Cryptopp 8.6.0)
if(Cryptopp_FOUND)
  include_directories(${cryptopp_INCLUDE_DIRS})
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(CopyFile)
add_subdirectory(CopyFileLib)
add_subdirectory(GoogleTests)
add_subdirectory(CopyOverNetwork)
