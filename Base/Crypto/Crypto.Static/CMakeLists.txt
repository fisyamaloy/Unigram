set(TARGET Crypto.Static)

set(SOURCE_FILES
    Cryptography.hpp
    Cryptography.cpp)

add_library(${TARGET} STATIC ${SOURCE_FILES})

find_package(cryptopp)
target_link_libraries(${TARGET} cryptopp::cryptopp)
