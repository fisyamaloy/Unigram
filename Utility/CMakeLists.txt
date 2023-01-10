project(Utility LANGUAGES CXX)

set(TARGET Utility)

add_subdirectory(Utility.Public)

add_library(${TARGET} INTERFACE)
