message(STATUS "Configuring Yulduz::Window")

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)

add_library(YulduzWindow STATIC ${SOURCES})
add_library(Yulduz::Window ALIAS YulduzWindow)

target_include_directories(YulduzWindow PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(YulduzWindow PUBLIC Yulduz::ThirdParty Yulduz::Core)

target_precompile_headers(YulduzWindow REUSE_FROM YulduzCore)