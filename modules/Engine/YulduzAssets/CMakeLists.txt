file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)

add_library(YulduzAssets STATIC ${SOURCES})
target_include_directories(YulduzAssets PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(YulduzAssets PUBLIC YulduzCore YulduzThirdParty)
target_precompile_headers(YulduzAssets REUSE_FROM YulduzCore)