project(Network LANGUAGES CXX)

set(TARGET Network)

add_subdirectory(Public)

set(TARGET Network)

add_library(${TARGET} INTERFACE)
