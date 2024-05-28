#include <wgpu/wgpu_impl_glfw.h>

WGPUSurface glfwGetWGPUSurface(GLFWwindow *window, WGPUInstance instance) {
#if defined(GLFW_EXPOSE_NATIVE_WIN32)
    HINSTANCE hInstance = GetModuleHandle(nullptr);
    HWND hWnd = glfwGetWin32Window(window);
    WGPUSurfaceDescriptorFromWindowsHWND surfaceFromHwnd{
        .chain = WGPUChainedStruct{.sType = WGPUSType_SurfaceDescriptorFromWindowsHWND},
        .hinstance = hInstance,
        .hwnd = hWnd,
    };
    WGPUSurfaceDescriptor surfaceDescriptor{
        .nextInChain = (WGPUChainedStruct *)&surfaceFromHwnd,
        .label = "Yulduz Surface for Windows",
    };
#elif defined(GLFW_EXPOSE_NATIVE_COCOA)
    WGPUSurfaceDescriptorFromMetalLayer surfaceFromMetalLayer{
        .chain = WGPUChainedStruct{.sType = WGPUSType_SurfaceDescriptorFromMetalLayer},
        .layer = glfwGetCocoaWindow(m_Window),
    };
    WGPUSurfaceDescriptor surfaceDescriptor{
        .nextInChain = (WGPUChainedStruct *)&surfaceFromMetalLayer,
        .label = "Yulduz Surface for macOS",
    };
#elif defined(GLFW_EXPOSE_NATIVE_X11)
    Display *x11_display = glfwGetX11Display();
    Window x11_window = glfwGetX11Window(window);
    WGPUSurfaceDescriptorFromXlibWindow surfaceFromXlibWindow{
        .chain = WGPUChainedStruct{.sType = WGPUSType_SurfaceDescriptorFromXlibWindow},
        .display = x11_display,
        .window = x11_window,
    };
    WGPUSurfaceDescriptor surfaceDescriptor{
        .nextInChain = (WGPUChainedStruct *)&surfaceFromXlibWindow,
        .label = "Yulduz Surface for Linux",
    };
#else
#error Not supported platform
#endif

    return wgpuInstanceCreateSurface(instance, &surfaceDescriptor);
}