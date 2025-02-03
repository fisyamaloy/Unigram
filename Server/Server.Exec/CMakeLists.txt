set(TARGET Server.Exec)
set(OUTPUT_NAME Juniorgram.Server.Exec)

set(SOURCE_FILES
    MainServer.cpp)

if(WIN32)
    set(EXTRA_LIBS ws2_32 wsock32)
endif()

add_executable(${TARGET} ${SOURCE_FILES})

find_package(Boost CONFIG REQUIRED COMPONENTS asio)
find_package(OpenSSL CONFIG REQUIRED)

#find_path(YAS_INCLUDE_DIRS "yas/abseil_types.hpp")
target_link_libraries(${TARGET} PRIVATE ${EXTRA_LIBS} Boost::asio)
target_link_libraries(${TARGET} PUBLIC Server.Core.Static)
target_link_libraries(${TARGET} PUBLIC Network.Public Utility.Public)
target_link_libraries(${TARGET} PRIVATE OpenSSL::SSL OpenSSL::Crypto)

install(TARGETS ${TARGET}
        CONFIGURATIONS Debug
        RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bin/Server/Debug)
install(TARGETS ${TARGET}
        CONFIGURATIONS Release
        RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bin/Server/Release)
