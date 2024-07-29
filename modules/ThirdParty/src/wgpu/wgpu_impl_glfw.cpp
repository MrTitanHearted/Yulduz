#include <wgpu/wgpu_impl_glfw.h>

#if defined(_WIN32) || defined(_WIN64)
#define GLFW_EXPOSE_NATIVE_WIN32
#define WIN32_LEAN_AND_MEAN
#elif defined(__linux__)
#define GLFW_EXPOSE_NATIVE_X11
#else
#error Unsupported Platform!
#endif

#include <GLFW/glfw3native.h>

WGPUSurface glfwGetWGPUSurface(GLFWwindow *window, WGPUInstance instance) {
#if defined(_WIN32) || defined(_WIN64)
    HINSTANCE hInstance = GetModuleHandle(nullptr);
    HWND hWnd = glfwGetWin32Window(window);
    WGPUSurfaceDescriptorFromWindowsHWND surfaceFromHWND{
        .chain = WGPUChainedStruct{.sType = WGPUSType_SurfaceDescriptorFromWindowsHWND},
        .hinstance = hInstance,
        .hwnd = hWnd,
    };
    WGPUSurfaceDescriptor surfaceDescriptor{
        .nextInChain = &surfaceFromHWND.chain,
        .label = "Yulduz Surface for Windows",
    };
    return wgpuInstanceCreateSurface(instance, &surfaceDescriptor);
#elif defined(__linux__)
    Display *x11_display = glfwGetX11Display();
    Window x11_window = glfwGetX11Window(window);
    WGPUSurfaceDescriptorFromXlibWindow surfaceFromXlibWindow{
        .chain = WGPUChainedStruct{.sType = WGPUSType_SurfaceDescriptorFromXlibWindow},
        .display = x11_display,
        .window = x11_window,
    };
    WGPUSurfaceDescriptor surfaceDescriptor{
        .nextInChain = &surfaceFromXlibWindow.chain,
        .label = "Yulduz Surface for Linux",
    };
    return wgpuInstanceCreateSurface(instance, &surfaceDescriptor);
#else
#error Unsupported Platform!
#endif
}