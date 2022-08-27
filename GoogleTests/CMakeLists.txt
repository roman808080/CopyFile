find_package(GTest)
find_package(cryptopp 8.6.0)

file(GLOB SRCS *.cpp)

ADD_EXECUTABLE(GoogleTests ${SRCS})

TARGET_LINK_LIBRARIES(GoogleTests
    CopyFileLib
    GTest::gtest
    GTest::gmock
    cryptopp-static
)

add_test(NAME GoogleTests
         COMMAND GoogleTests)
