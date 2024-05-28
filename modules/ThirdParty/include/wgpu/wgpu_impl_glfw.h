#pragma once

#include <GLFW/glfw3.h>
#include <wgpu/wgpu.h>

#if defined(_WIN32) || defined(_WIN64)
#define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(__APPLE__) || defined(__MACH__)
#define GLFW_EXPOSE_NATIVE_COCOA
#elif defined(__linux__)
#define GLFW_EXPOSE_NATIVE_X11
#else
#error Not supported platform
#endif

#include <GLFW/glfw3native.h>

struct WGPUAdapterImpl {};
struct WGPUBindGroupImpl {};
struct WGPUBindGroupLayoutImpl {};
struct WGPUBufferImpl {};
struct WGPUCommandBufferImpl {};
struct WGPUCommandEncoderImpl {};
struct WGPUComputePassEncoderImpl {};
struct WGPUComputePipelineImpl {};
struct WGPUDeviceImpl {};
struct WGPUInstanceImpl {};
struct WGPUPipelineLayoutImpl {};
struct WGPUQuerySetImpl {};
struct WGPUQueueImpl {};
struct WGPURenderBundleImpl {};
struct WGPURenderBundleEncoderImpl {};
struct WGPURenderPassEncoderImpl {};
struct WGPURenderPipelineImpl {};
struct WGPUSamplerImpl {};
struct WGPUShaderModuleImpl {};
struct WGPUSurfaceImpl {};
struct WGPUTextureImpl {};
struct WGPUTextureViewImpl {};

WGPUSurface glfwGetWGPUSurface(GLFWwindow *window, WGPUInstance instance);