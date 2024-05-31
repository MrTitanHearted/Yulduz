cmake_minimum_required(VERSION 3.20)

list(APPEND CMAKE_TOOLCHAIN_FILE $ENV{VCPKG_CMAKE})

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_compile_definitions(YULDUZ_SOURCE_DIR="${CMAKE_SOURCE_DIR}")

project(Yulduz LANGUAGES CXX C VERSION 1.0.0)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

if(BUILD_SHARED_LIBS AND WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

if(CMAKE_BUILD_TYPE MATCHES Release)
    add_compile_definitions(YULDUZ_BUILD_TYPE_RELEASE)

    if(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
        add_compile_options(/O2)
    else()
        add_compile_options(-O3)
    endif()
else()
    add_compile_definitions(YULDUZ_BUILD_TYPE_DEBUG)

    if(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
        add_compile_options(/Zi /Od)
    else()
        add_compile_options(-g)
    endif()
endif()

add_subdirectory(modules)