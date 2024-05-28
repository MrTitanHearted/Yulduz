cmake_minimum_required(VERSION 3.20)

list(APPEND CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/wgpu-cmake)

if(WIN32)
    set(VCPKG $ENV{VCPKG_ROOT}/installed/x64-windows-static)
elseif(APPLE)
    set(VCPKG $ENV{VCPKG_ROOT}/installed/x64-osx-static)
elseif(UNIX)
    set(VCPKG $ENV{VCPKG_ROOT}/installed/x64-linux-static)
else()
    message(FATAL_ERROR "Not supported platform!")
endif()

project(YulduzThirdParty)

find_package(wgpu CONFIG REQUIRED)

file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
)

add_library(YulduzThirdParty STATIC ${SOURCES})

target_include_directories(YulduzThirdParty PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${VCPKG}/include
)

if(CMAKE_BUILD_TYPE MATCHES Release)
    target_link_directories(YulduzThirdParty PUBLIC ${VCPKG}/lib)
else()
    target_link_directories(YulduzThirdParty PUBLIC ${VCPKG}/debug/lib)
endif()

target_link_libraries(YulduzThirdParty PUBLIC
    wgpu::wgpu
    glm
    glfw3
)

if(NOT CMAKE_BUILD_TYPE MATCHES Release)
    target_link_libraries(YulduzThirdParty PUBLIC Dwmapi assimp-vc143-mtd)
else()
    target_link_libraries(YulduzThirdParty PUBLIC assimp-vc143-mt)
endif()