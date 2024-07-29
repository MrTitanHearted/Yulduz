set(WGPU_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include)

if(CMAKE_BUILD_TYPE MATCHES Release)
    set(WGPU_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/lib/Release)
else()
    set(WGPU_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/lib/Debug)
endif()

add_library(wgpu INTERFACE IMPORTED)
add_library(wgpu::wgpu ALIAS wgpu)

target_include_directories(wgpu INTERFACE ${WGPU_INCLUDE_DIR})
target_link_directories(wgpu INTERFACE ${WGPU_LIBRARY_DIR})
target_link_libraries(wgpu INTERFACE wgpu_native d3dcompiler ws2_32 userenv bcrypt ntdll d3d12 dxgi dxguid opengl32)