message(STATUS "Configuring Yulduz::Window")

file(GLOB_RECURSE SOURCES src/*.cpp include/*.hpp)

add_library(YulduzWindow STATIC ${SOURCES})
add_library(Yulduz::Window ALIAS YulduzWindow)

target_include_directories(YulduzWindow PUBLIC include)

target_link_libraries(YulduzWindow PUBLIC Yulduz::ThirdParty Yulduz::Core)

target_precompile_headers(YulduzWindow REUSE_FROM YulduzCore)