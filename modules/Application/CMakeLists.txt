cmake_minimum_required(VERSION 3.20)

project(Yulduz)

file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)

if(CMAKE_BUILD_TYPE MATCHES Release)
    add_executable(Yulduz WIN32 ${SOURCES})
else()
    add_executable(Yulduz ${SOURCES})
endif()

target_include_directories(Yulduz PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(Yulduz PRIVATE YulduzEngine)