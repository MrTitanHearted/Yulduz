message(STATUS "Configuring Yulduz::Engine")

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)

if(BUILD_SHARED_LIBS)
    add_library(YulduzEngine SHARED ${SOURCES})
else()
    add_library(YulduzEngine STATIC ${SOURCES})
endif()

add_library(Yulduz::Engine ALIAS YulduzEngine)

target_include_directories(YulduzEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(YulduzEngine PUBLIC Yulduz::ThirdParty Yulduz::Core Yulduz::Assets Yulduz::Window Yulduz::Render)

target_precompile_headers(YulduzEngine REUSE_FROM YulduzCore)