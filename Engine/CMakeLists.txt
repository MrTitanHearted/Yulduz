cmake_minimum_required(VERSION 3.20)

project(YulduzEngine LANGUAGES CXX VERSION 0.0.1)

file(GLOB_RECURSE YULDUZ_INCLUDE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/*.h
)
file(GLOB_RECURSE YULDUZ_HIDDEN_INCLUDE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h
)
file(GLOB_RECURSE YULDUZ_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES
    ${YULDUZ_INCLUDE_FILES}
    ${YULDUZ_HIDDEN_INCLUDE_FILES}
    ${YULDUZ_SOURCE_FILES}
)

if (BUILD_SHARED_LIBS)
    set(YULDUZ_BUILD_LIBS_TYPE SHARED)
else ()
    set(YULDUZ_BUILD_LIBS_TYPE STATIC)
endif ()

add_library(YulduzEngine ${YULDUZ_BUILD_LIBS_TYPE}
    ${YULDUZ_INCLUDE_FILES}
    ${YULDUZ_SOURCE_FILES}
)
add_library(Yulduz::Engine ALIAS YulduzEngine)

if (BUILD_SHARED_LIBS)
    target_compile_definitions(YulduzEngine PUBLIC YULDUZ_SHARED)
else ()
    target_compile_definitions(YulduzEngine PUBLIC YULDUZ_STATIC)
endif ()

target_compile_definitions(YulduzEngine PRIVATE YULDUZ_BUILD)

target_include_directories(YulduzEngine
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

target_link_libraries(YulduzEngine
    PUBLIC Yulduz::Vendor
)

target_compile_definitions(YulduzEngine PUBLIC
    GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
    GLM_FORCE_RADIANS
    GLM_FORCE_DEPTH_ZERO_TO_ONE
    GLM_ENABLE_EXPERIMENTAL
)

target_precompile_headers(YulduzEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include/Yulduz/Common/PCH.hpp)

set_property(TARGET YulduzEngine PROPERTY FOLDER "Engine")