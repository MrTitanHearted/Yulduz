cmake_minimum_required(VERSION 3.20)

list(APPEND CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/wgpu-cmake)

project(YulduzThirdParty)

find_package(wgpu CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(Stb REQUIRED)

file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
)

add_library(YulduzThirdParty STATIC ${SOURCES})

target_include_directories(YulduzThirdParty PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${Stb_INCLUDE_DIR}
)

target_link_libraries(YulduzThirdParty PUBLIC
    wgpu::wgpu
    glm::glm-header-only
    glfw
    assimp::assimp
    EnTT::EnTT
)

if(NOT CMAKE_BUILD_TYPE MATCHES Release)
    target_link_libraries(YulduzThirdParty PUBLIC Dwmapi)
endif()