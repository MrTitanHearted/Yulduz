message(STATUS "Configuring Yulduz::Assets")

file(GLOB_RECURSE SOURCES src/*.cpp include/*.hpp)

add_library(YulduzAssets STATIC ${SOURCES})
add_library(Yulduz::Assets ALIAS YulduzAssets)

target_include_directories(YulduzAssets PUBLIC include)

target_link_libraries(YulduzAssets PUBLIC Yulduz::ThirdParty Yulduz::Core)

target_precompile_headers(YulduzAssets REUSE_FROM YulduzCore)