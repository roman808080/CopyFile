cmake_minimum_required(VERSION 2.8)

set(BOOST_ROOT "/home/ubuntu/boost/")
set(BOOST_INCLUDEDIR "/home/ubuntu/boost/include")
set(BOOST_LIBRARYDIR "/home/ubuntu/boost/lib")

find_package(Boost 1.74.0)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  message(WARNING ${Boost_INCLUDE_DIRS})
  message(WARNING "Hell")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(CopyFile)

add_subdirectory(CopyFile)
add_subdirectory(CopyFileLib)
add_subdirectory(GoogleTests)

