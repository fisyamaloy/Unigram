project(Base LANGUAGES CXX)

set(TARGET Base)

add_subdirectory(Logger)
add_subdirectory(Crypto)

add_library(${TARGET} INTERFACE)
