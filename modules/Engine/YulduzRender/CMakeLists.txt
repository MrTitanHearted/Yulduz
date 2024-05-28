file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)

add_library(YulduzRender STATIC ${SOURCES})
target_include_directories(YulduzRender PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(YulduzRender PUBLIC YulduzWindow YulduzAssets YulduzCore YulduzThirdParty)
target_precompile_headers(YulduzRender REUSE_FROM YulduzCore)