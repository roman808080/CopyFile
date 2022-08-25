find_package(GTest)
file(GLOB SRCS *.cpp)

ADD_EXECUTABLE(GoogleTests ${SRCS})

TARGET_LINK_LIBRARIES(GoogleTests
    CopyFileLib
    GTest::gtest
    GTest::gmock
)

add_test(NAME GoogleTests
         COMMAND GoogleTests)
