set(TARGET Client.Core.Static)

set(SOURCE_FILES
        Client.hpp
        Client.cpp
        ServerInfo.hpp)

if (WIN32)
    list(APPEND EXTRA_LIBS ws2_32 wsock32)
endif ()

add_library(${TARGET} STATIC ${SOURCE_FILES})

target_include_directories(${TARGET}
        INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${TARGET}
        PRIVATE ${EXTRA_LIBS} CONAN_PKG::asio CONAN_PKG::yas
        PUBLIC Network.Public Crypto.Static)

target_include_directories(${TARGET}
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${TARGET} PRIVATE
    $<TARGET_PROPERTY:Crypto.Static,SOURCE_DIR>/..)
