file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)

add_library(YulduzWindow STATIC ${SOURCES})
target_include_directories(YulduzWindow PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(YulduzWindow PUBLIC YulduzAssets YulduzCore YulduzThirdParty)
target_precompile_headers(YulduzWindow REUSE_FROM YulduzCore)