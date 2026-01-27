cmake_minimum_required(VERSION 3.20)

project(YulduzEditor LANGUAGES CXX VERSION 0.0.1)

if (WIN32)
    option(WIN32_RELEASE_MODE_NO_CONSOLE "Release mode for Win32" OFF)
endif ()

set(YULDUZ_ASSETS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/Assets)
set(YULDUZ_ASSETS_DESTINATION_DIR ${CMAKE_BINARY_DIR}/Assets)

file(GLOB_RECURSE YULDUZ_EDITOR_INCLUDE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/*.h
)
file(GLOB_RECURSE YULDUZ_EDITOR_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES
    ${YULDUZ_EDITOR_INCLUDE_FILES}
    ${YULDUZ_EDITOR_SOURCE_FILES}
)

add_executable(YulduzEditor
    ${YULDUZ_EDITOR_INCLUDE_FILES}
    ${YULDUZ_EDITOR_SOURCE_FILES}
)

target_include_directories(YulduzEditor
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

target_link_libraries(YulduzEditor PRIVATE Yulduz::Engine)

if (WIN32 AND WIN32_RELEASE_MODE_NO_CONSOLE)
    if (MSVC)
        set_target_properties(YulduzEditor PROPERTIES WIN32_EXECUTABLE $<$<CONFIG:Release>:TRUE>)
    elseif (CMAKE_BUILD_TYPE MATCHES Release)
        set_target_properties(YulduzEditor PROPERTIES WIN32_EXECUTABLE TRUE)
    endif ()
endif ()

if (MSVC)
    set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT YulduzEditor)
endif ()

find_program(SLANGC_EXECUTABLE slangc REQUIRED)

if (NOT SLANGC_EXECUTABLE)
    message(FATAL_ERROR "Slang compiler not found. Install slangc and add it to PATH.")
endif ()

file(GLOB_RECURSE YULDUZ_EDITOR_SHADERS ${YULDUZ_ASSETS_SOURCE_DIR}/Shaders/*.slang)

set(SPIRV_BINARY_FILES "")

foreach (SHADER_FILE ${YULDUZ_EDITOR_SHADERS})
    get_filename_component(FILE_NAME_WE ${SHADER_FILE} NAME_WE)
    get_filename_component(FILE_PATH ${SHADER_FILE} PATH)

    set(SPIRV "${FILE_PATH}/${FILE_NAME_WE}.spv")

    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${SLANGC_EXECUTABLE} -matrix-layout-column-major -fvk-use-entrypoint-name -emit-spirv-directly -I ${YULDUZ_ASSETS_SOURCE_DIR} ${SHADER_FILE} -target spirv -o ${SPIRV}
        DEPENDS ${SHADER_FILE}
    )

    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach ()

add_custom_target(YulduzEditorAssets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${YULDUZ_ASSETS_SOURCE_DIR} ${YULDUZ_ASSETS_DESTINATION_DIR}
    DEPENDS ${SPIRV_BINARY_FILES} ${YULDUZ_ASSETS_SOURCE_DIR}
    COMMENT "Copying '${YULDUZ_ASSETS_SOURCE_DIR}' folder to '${YULDUZ_ASSETS_DESTINATION_DIR}'"
)

add_dependencies(YulduzEditor YulduzEditorAssets)

file(GLOB_RECURSE YULDUZ_ASSETS_FILES ${YULDUZ_ASSETS_SOURCE_DIR}/*)
source_group(TREE ${YULDUZ_ASSETS_SOURCE_DIR} PREFIX Assets FILES ${YULDUZ_ASSETS_FILES})
target_sources(YulduzEditorAssets PRIVATE ${YULDUZ_ASSETS_FILES})

set_property(TARGET YulduzEditorAssets PROPERTY FOLDER "Editor")
set_property(TARGET YulduzEditor PROPERTY FOLDER "Editor")