message(STATUS "Configuring Yulduz::Core")

file(GLOB_RECURSE SOURCES src/*.cpp include/*.hpp)

add_library(YulduzCore STATIC ${SOURCES})
add_library(Yulduz::Core ALIAS YulduzCore)

target_include_directories(YulduzCore PUBLIC include)

target_link_libraries(YulduzCore PUBLIC Yulduz::ThirdParty)

target_precompile_headers(YulduzCore PUBLIC include/YulduzCore/YulduzPCH.hpp)