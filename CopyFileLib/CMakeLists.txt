set(copylib_SRCS
    App.h
    Constants.h
    InputFile.h
    OutputFile.h
    pch.h
    Router.h
    App.cpp
    InputFile.cpp
    OutputFile.cpp
    pch.cpp
    Router.cpp
    SharedMemory.cpp
)

# Declare the library
add_library(CopyFileLib STATIC
    ${copylib_SRCS}
)

# Specify here the include directories exported
# by this library
target_include_directories(CopyFileLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
