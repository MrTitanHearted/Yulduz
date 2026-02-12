cmake_minimum_required(VERSION 3.20)

file(GLOB_RECURSE YULDUZ_INCLUDE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/*.h
)
file(GLOB_RECURSE YULDUZ_HIDDEN_INCLUDE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h
)
file(GLOB_RECURSE YULDUZ_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.c
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
    target_compile_definitions(YulduzEngine PUBLIC YULDUZ_ENGINE_SHARED)
else ()
    target_compile_definitions(YulduzEngine PUBLIC YULDUZ_ENGINE_STATIC)
endif ()

target_compile_definitions(YulduzEngine PRIVATE YULDUZ_ENGINE_BUILD)

target_include_directories(YulduzEngine
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

target_link_libraries(YulduzEngine PUBLIC Yulduz::Vendor)

target_precompile_headers(YulduzEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include/Yulduz/Common/PCH.h)

set_property(TARGET YulduzEngine PROPERTY FOLDER "Engine")