project(Client.Core LANGUAGES CXX)

add_subdirectory(Client.Core.Static)

set(TARGET Client.Core)

add_library(${TARGET} INTERFACE)
