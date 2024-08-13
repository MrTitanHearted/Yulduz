cmake_minimum_required(VERSION 3.20)

list(APPEND CMAKE_TOOLCHAIN_FILE $ENV{VCPKG_CMAKE})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_compile_definitions(YULDUZ_SOURCE_DIR="${CMAKE_SOURCE_DIR}")

project(Yulduz LANGUAGES CXX C VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(ENGINE_ONLY "Build the Yulduz Engine libraries only" OFF)

if(BUILD_SHARED_LIBS AND WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

if(MSVC)
    add_compile_options(/MP)

    add_compile_definitions(
        $<$<CONFIG:Release>:YULDUZ_BUILD_TYPE_RELEASE>
        $<$<CONFIG:Debug>:YULDUZ_BUILD_TYPE_DEBUG>
    )

    add_compile_options(
        $<$<CONFIG:Release>:/O2>
        $<$<CONFIG:Debug>:/Zi>
        $<$<CONFIG:Debug>:/Od>
    )
else()
    if(CMAKE_BUILD_TYPE MATCHES Release)
        add_compile_definitions(YULDUZ_BUILD_TYPE_RELEASE)
        add_compile_options(-O3)
    else()
        add_compile_definitions(YULDUZ_BUILD_TYPE_DEBUG)
        add_compile_options(-g)
    endif()
endif()

add_subdirectory(modules)