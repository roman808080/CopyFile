set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(libfoo_SRCS
    App.h
    Constants.h
    framework.h
    InputFile.h
    OutputFile.h
    pch.h
    Router.h
    App.cpp
    InputFile.cpp
    OutputFile.cpp
    pch.cpp
    Router.cpp
)

# Declare the library
add_library(CopyFileLib STATIC
    ${libfoo_SRCS}
)

# Specify here the include directories exported
# by this library
target_include_directories(CopyFileLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
