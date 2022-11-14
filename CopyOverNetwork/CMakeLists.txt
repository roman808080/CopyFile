set(Main_SRCS
    CopyFileOverNetwork.cpp
    NetworkApp.cpp
    Client.cpp
    Server.cpp
)

add_executable(CopyFileOverNetwork
    ${Main_SRCS}
)

# Specify here the libraries this program depends on
target_link_libraries(CopyFileOverNetwork
    CopyFileLib
    Boost::program_options
    cryptopp-static
)

install(TARGETS CopyFileOverNetwork DESTINATION bin)
