cmake_minimum_required(VERSION 3.20)

add_subdirectory(SDL)
add_subdirectory(SDL_image)
add_subdirectory(SDL_mixer)
add_subdirectory(SDL_net)
add_subdirectory(SDL_ttf)

file(GLOB YULDUZ_STB_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/stb/*.h)

file(GLOB_RECURSE YULDUZ_VENDOR_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/Yulduz/Include/*.h)
file(GLOB_RECURSE YULDUZ_VENDOR_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Yulduz/Source/*.c)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/stb PREFIX Include FILES ${YULDUZ_STB_HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Yulduz/Include PREFIX Include FILES ${YULDUZ_VENDOR_HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Yulduz/Source PREFIX Source FILES ${YULDUZ_VENDOR_SOURCES})

add_library(YulduzVendor STATIC
    ${YULDUZ_STB_HEADERS}
    ${YULDUZ_VENDOR_HEADERS}
    ${YULDUZ_VENDOR_SOURCES}
)
add_library(Yulduz::Vendor ALIAS YulduzVendor)

target_link_libraries(YulduzVendor PUBLIC
    SDL3::SDL3
    SDL3_image::SDL3_image
    SDL3_mixer::SDL3_mixer
    SDL3_net::SDL3_net
    SDL3_ttf::SDL3_ttf
)

target_include_directories(YulduzVendor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/stb
    ${CMAKE_CURRENT_SOURCE_DIR}/Yulduz/Include
)

set_property(TARGET YulduzVendor PROPERTY FOLDER "Vendor")