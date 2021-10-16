project(CMakeProject1 LANGUAGES CXX)

add_subdirectory(Client.Core)
add_subdirectory(Client.Qt)

set(TARGET CMakeProject1)
add_library(${TARGET} INTERFACE)
