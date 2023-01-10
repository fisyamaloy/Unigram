project(DataAccess LANGUAGES CXX)

set(TARGET DataAccess)

add_subdirectory(Public)
add_subdirectory(DataAccess.Postgre)

add_library(${TARGET} INTERFACE)
