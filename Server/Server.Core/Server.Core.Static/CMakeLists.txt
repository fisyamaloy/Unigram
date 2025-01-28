set(TARGET Server.Core.Static)
set(OUTPUT_NAME Server.Core.Static)

set(SOURCE_FILES
    Server.hpp
	Server.cpp)

if(WIN32)
	list(APPEND EXTRA_LIBS ws2_32 wsock32)
endif()

add_library(${TARGET} STATIC ${SOURCE_FILES})

target_include_directories(${TARGET}
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${TARGET} PRIVATE
    $<TARGET_PROPERTY:DataAccess.Postgre,SOURCE_DIR>/..)

find_package(Boost CONFIG REQUIRED COMPONENTS asio)
find_package(PostgreSQL)

#find_path(YAS_INCLUDE_DIRS "yas/abseil_types.hpp")
target_link_libraries(${TARGET} PRIVATE ${EXTRA_LIBS} Boost::asio DataAccess.Postgre)
target_link_libraries(${TARGET} PUBLIC Network.Public DataAccess.Public Utility.Public)
