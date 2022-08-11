set(Main_SRCS
    CopyFile.cpp
)

add_executable(CopyFile
    ${Main_SRCS}
)

# Specify here the libraries this program depends on
target_link_libraries(CopyFile
    CopyFileLib
)

install(TARGETS CopyFile DESTINATION bin)
