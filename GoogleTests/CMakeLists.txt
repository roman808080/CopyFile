find_package(GTest)
file(GLOB SRCS *.cpp)

ADD_EXECUTABLE(GoogleTests ${SRCS})

TARGET_LINK_LIBRARIES(GoogleTests
    CopyFileLib
    gtest::gtest
)

add_test(NAME GoogleTests
         COMMAND GoogleTests)
