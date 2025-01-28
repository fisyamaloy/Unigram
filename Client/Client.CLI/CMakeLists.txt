set(TARGET Client.CLI)
 
set(SOURCE_FILES
   MainClient.cpp
   App.cpp
   App.hpp)
 
add_executable(${TARGET} ${SOURCE_FILES})

find_package(Boost CONFIG REQUIRED COMPONENTS asio)

target_link_libraries(${TARGET} PUBLIC Network.Public Utility.Public)
target_link_libraries(${TARGET} PUBLIC Client.Core.Static)
target_link_libraries(${TARGET} PRIVATE ${EXTRA_LIBS} Boost::asio)

install(TARGETS ${TARGET}
        CONFIGURATIONS Debug
        RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bin/Client/Debug)
install(TARGETS ${TARGET}
        CONFIGURATIONS Release
        RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bin/Client/Release)
