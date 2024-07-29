message(STATUS "Configuring Yulduz::Render")

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)

add_library(YulduzRender STATIC ${SOURCES})
add_library(Yulduz::Render ALIAS YulduzRender)

target_include_directories(YulduzRender PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(YulduzRender PUBLIC Yulduz::ThirdParty Yulduz::Core Yulduz::Assets Yulduz::Window)

target_precompile_headers(YulduzRender REUSE_FROM YulduzCore)