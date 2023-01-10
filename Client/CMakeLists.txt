project(Client LANGUAGES CXX)

set(TARGET Client)

add_subdirectory(Client.Core)
add_subdirectory(Client.Qt)
add_subdirectory(Client.CLI)

add_library(${TARGET} INTERFACE)
