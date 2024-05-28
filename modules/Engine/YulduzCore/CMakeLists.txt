file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)

add_library(YulduzCore STATIC ${SOURCES})
target_include_directories(YulduzCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(YulduzCore PUBLIC YulduzThirdParty)
target_precompile_headers(YulduzCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/YulduzCore/YulduzPCH.hpp)