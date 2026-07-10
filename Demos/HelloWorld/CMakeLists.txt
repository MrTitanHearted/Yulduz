cmake_minimum_required(VERSION 3.20)

if (WIN32)
    option(WIN32_RELEASE_MODE_NO_CONSOLE "Release mode for Win32" OFF)
endif ()

file(GLOB_RECURSE YULDUZ_APPS_HELLO_WORLD_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/Include/*.h)
file(GLOB_RECURSE YULDUZ_APPS_HELLO_WORLD_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.c)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES
    ${YULDUZ_APPS_HELLO_WORLD_HEADERS}
    ${YULDUZ_APPS_HELLO_WORLD_SOURCES}
)

add_executable(YulduzHelloWorld
    ${YULDUZ_APPS_HELLO_WORLD_HEADERS}
    ${YULDUZ_APPS_HELLO_WORLD_SOURCES}
)

target_include_directories(YulduzHelloWorld PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Include)

target_link_libraries(YulduzHelloWorld PRIVATE Yulduz::Engine)

target_precompile_headers(YulduzHelloWorld REUSE_FROM Yulduz::Engine)

if (WIN32 AND WIN32_RELEASE_MODE_NO_CONSOLE)
    if (MSVC)
        set_target_properties(YulduzHelloWorld PROPERTIES WIN32_EXECUTABLE $<$<CONFIG:Release>:TRUE>)
    elseif (CMAKE_BUILD_TYPE MATCHES Release)
        set_target_properties(YulduzHelloWorld PROPERTIES WIN32_EXECUTABLE TRUE)
    endif ()
endif ()

if (MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT YulduzHelloWorld)
endif ()

set_property(TARGET YulduzHelloWorld PROPERTY FOLDER "YulduzHelloWorld")