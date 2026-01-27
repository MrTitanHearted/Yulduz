cmake_minimum_required(VERSION 3.20)

project(YulduzVendor)

add_subdirectory(SDL)

find_package(EASTL CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(mikktspace CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)

set(YULDUZ_THIRD_PARTY_IMGUI_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imconfig.h
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_internal.h
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.h
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imstb_rectpack.h
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imstb_textedit.h
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imstb_truetype.h

    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_sdl3.h
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_sdlgpu3.h
)

set(YULDUZ_THIRD_PARTY_IMGUI_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_demo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_draw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_tables.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_widgets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_sdl3.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_sdlgpu3.cpp
)

file(GLOB YULDUZ_THIRD_PARTY_STB_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/stb/*.h)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/imgui PREFIX Include FILES ${YULDUZ_THIRD_PARTY_IMGUI_HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/imgui PREFIX Source FILES ${YULDUZ_THIRD_PARTY_IMGUI_SOURCES})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/stb PREFIX Include FILES ${YULDUZ_THIRD_PARTY_STB_HEADERS})

add_library(YulduzVendor STATIC
    ${YULDUZ_THIRD_PARTY_IMGUI_HEADERS}
    ${YULDUZ_THIRD_PARTY_IMGUI_SOURCES}
)
add_library(Yulduz::Vendor ALIAS YulduzVendor)

target_link_libraries(YulduzVendor PUBLIC
    EASTL
    glm::glm
    spdlog::spdlog
    mikktspace::mikktspace
    assimp::assimp
    SDL3::SDL3
)

target_include_directories(YulduzVendor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/stb
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui
)

set_property(TARGET YulduzVendor PROPERTY FOLDER "Vendor")
