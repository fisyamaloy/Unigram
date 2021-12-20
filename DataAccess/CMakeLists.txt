project(DataAccess LANGUAGES CXX)

add_subdirectory(Public)
add_subdirectory(DataAccess.Postgre)

set(TARGET DataAccess)

add_library(${TARGET} INTERFACE)
