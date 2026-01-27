cmake_minimum_required(VERSION 3.20)

list(APPEND CMAKE_TOOLCHAIN_FILE $ENV{VCPKG_CMAKE})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if (MSVC)
else ()
    if (NOT DEFINED CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type: Debug, RelWithDebInfo, Release")
    endif ()

    string(TOUPPER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_UPPER)

    if (NOT CMAKE_BUILD_TYPE_UPPER MATCHES "^(DEBUG|RELWITHDEBINFO|RELEASE)$")
        message(FATAL_ERROR "Invalid build type: ${CMAKE_BUILD_TYPE}")
    elseif (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_BUILD_TYPE Debug)
    elseif (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        set(CMAKE_BUILD_TYPE RelWithDebInfo)
    elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
        set(CMAKE_BUILD_TYPE Release)
    endif ()
endif ()

project(Yulduz LANGUAGES CXX C VERSION 0.0.1)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 23)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

if (BUILD_SHARED_LIBS AND WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif ()

if (MSVC)
    add_compile_options(/MP)

    add_compile_definitions(
        $<$<CONFIG:Release>:YULDUZ_BUILD_TYPE_RELEASE>
        $<$<CONFIG:Release>:NDEBUG>
        $<$<CONFIG:RelWithDebInfo>:YULDUZ_BUILD_TYPE_RELWITHDEBINFO>
        $<$<CONFIG:RelWithDebInfo>:DEBUG>
        $<$<CONFIG:Debug>:YULDUZ_BUILD_TYPE_DEBUG>
        $<$<CONFIG:Debug>:DEBUG>
    )

    add_compile_options(
        $<$<CONFIG:Release>:/O2>
        $<$<CONFIG:Release>:/Qpar>
        $<$<CONFIG:RelWithDebInfo>:/O2>
        $<$<CONFIG:RelWithDebInfo>:/Qpar>
        $<$<CONFIG:RelWithDebInfo>:/Zi>
        $<$<CONFIG:RelWithDebInfo>:/Od>
        $<$<CONFIG:Debug>:/Zi>
        $<$<CONFIG:Debug>:/Od>
    )
else ()
    if (CMAKE_BUILD_TYPE MATCHES Release)
        add_compile_definitions(YULDUZ_BUILD_TYPE_RELEASE NDEBUG)
        add_compile_options(-O3 -funroll-loops)
    elseif (CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
        add_compile_definitions(YULDUZ_BUILD_TYPE_RELWITHDEBINFO DEBUG)
        add_compile_options(-O3 -funroll-loops -g -pthread)
    else ()
        add_compile_definitions(YULDUZ_BUILD_TYPE_DEBUG DEBUG)
        add_compile_options(-g -pthread)
    endif ()
endif ()

add_compile_definitions(YULDUZ_PROJECT_ROOT_DIR="${CMAKE_SOURCE_DIR}")

add_subdirectory(Vendor)
add_subdirectory(Engine)
add_subdirectory(Editor)