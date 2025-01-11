project(Logger LANGUAGES CXX)

add_subdirectory(Public)
add_subdirectory(Logger.Static)

set(TARGET Logger)

add_library(${TARGET} INTERFACE)
