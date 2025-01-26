cmake_minimum_required(VERSION 3.30.0)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "c:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Path to vcpkg toolchain file")
endif()

if(NOT DEFINED VCPKG_TARGET_TRIPLET)
    set(VCPKG_TARGET_TRIPLET "x64-windows" CACHE STRING "Vcpkg target triplet")
endif()

project(Unigram)

# Global settings. Should probably be moved to a separate file later.
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CXX_EXTENSIONS off)

# For those VSCode users
set(EXPORT_COMPILE_COMMANDS TRUE)

# Set warnings level higher and force treating them as errors.
# GCC TBD
if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4 /WX)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

enable_testing()

add_subdirectory(Base)
add_subdirectory(Client)
add_subdirectory(DataAccess)
add_subdirectory(Network)
add_subdirectory(Server)
add_subdirectory(Utility)
