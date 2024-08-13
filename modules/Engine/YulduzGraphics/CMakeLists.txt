message(STATUS "Configuring Yulduz::Graphics")

file(GLOB_RECURSE SOURCES src/*.cpp include/*.hpp)

add_library(YulduzGraphics STATIC ${SOURCES})
add_library(Yulduz::Graphics ALIAS YulduzGraphics)

target_include_directories(YulduzGraphics PUBLIC include)

target_link_libraries(YulduzGraphics PUBLIC Yulduz::ThirdParty Yulduz::Core Yulduz::Assets Yulduz::Window)

target_precompile_headers(YulduzGraphics REUSE_FROM YulduzCore)