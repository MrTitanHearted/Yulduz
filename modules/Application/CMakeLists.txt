cmake_minimum_required(VERSION 3.20)

project(Yulduz)

file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})

if(MSVC)
    file(GLOB_RECURSE ASSET_FILES ${CMAKE_SOURCE_DIR}/assets/*)
    add_custom_target(Assets SOURCES ${ASSET_FILES})
    source_group(TREE ${CMAKE_SOURCE_DIR}/assets PREFIX "assets" FILES ${ASSET_FILES})
endif()

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)

add_executable(Yulduz ${SOURCES})

if(MSVC)
    set_target_properties(Yulduz PROPERTIES WIN32_EXECUTABLE $<$<CONFIG:Release>:TRUE>)
endif()

target_include_directories(Yulduz PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(Yulduz PRIVATE Yulduz::Engine)

if(MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Yulduz)
endif()