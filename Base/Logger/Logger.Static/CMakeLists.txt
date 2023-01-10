set(TARGET Base.Logger.Static)

set(SOURCE_FILES
        FileLogger.hpp
        FileLogger.cpp)

find_package(Threads REQUIRED)

add_library(${TARGET} STATIC ${SOURCE_FILES})

target_link_libraries(${TARGET} PUBLIC
        Base.Logger.Public
        Utility.Public)
