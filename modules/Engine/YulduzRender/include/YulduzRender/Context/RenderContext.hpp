#pragma once

#include <Yulduz/Core.hpp>
#include <Yulduz/Window.hpp>
#include <YulduzRender/Enums.hpp>
#include <YulduzRender/Context/Texture.hpp>

namespace Yulduz {
    class RenderContext : public std::enable_shared_from_this<RenderContext> {
       public:
        RenderContext(std::shared_ptr<Window> &window,
                      const std::string &label,
                      WGPUInstance instance,
                      WGPUSurface surface,
                      WGPUAdapter adapter,
                      WGPUDevice device,
                      WGPUSurfaceConfiguration config);
        ~RenderContext();

        void registerCallbacks(EventDispatcher &eventDispatcher);
        void resize(uint32_t width, uint32_t height);
        void renderFrameOnSurface(std::function<void(const std::shared_ptr<Framebuffer> &textrue)> frameCallback);
        void submitCommands(const std::vector<std::shared_ptr<CommandBuffer>> &commands) const;
        void printWGPUReport() const;

        inline std::string getLabel() const { return m_Label; }
        inline WGPUInstance getInstance() const { return m_Instance; }
        inline WGPUSurface getSurface() const { return m_Surface; }
        inline WGPUAdapter getAdapter() const { return m_Adapter; }
        inline WGPUDevice getDevice() const { return m_Device; }
        inline WGPUQueue getQueue() const { return m_Queue; }
        inline WGPUSurfaceConfiguration getConfig() const { return m_Config; }
        inline WGPULimits getLimits() const { return m_Limits; }
        inline std::shared_ptr<Framebuffer> const getDepthTexture() const { return m_DepthTexture; }
        inline std::shared_ptr<Window> getWindow() const { return m_Window; }
        inline TextureFormat getSurfaceFormat() const { return static_cast<TextureFormat>(m_Config.format); }

        static void SetupWGPULogging(WebGPULogLevel level);

       private:
        std::string m_Label;
        WGPUInstance m_Instance;
        WGPUSurface m_Surface;
        WGPUAdapter m_Adapter;
        WGPUDevice m_Device;
        WGPUQueue m_Queue;
        WGPUSurfaceConfiguration m_Config;
        WGPULimits m_Limits;
        std::shared_ptr<Framebuffer> m_DepthTexture;
        std::shared_ptr<Window> m_Window;

       private:
        void windowResizeCallback(const WindowResizeEvent &event);

        static void WGPULogCallback(WGPULogLevel level, const char *message, void *data);
    };

    class RenderContextBuilder {
       public:
        RenderContextBuilder();
        ~RenderContextBuilder() = default;

        inline static RenderContextBuilder New() { return RenderContextBuilder(); }

        RenderContextBuilder &setLabel(const std::string &label);
        RenderContextBuilder &setDeviceLabel(const std::string &label);
        RenderContextBuilder &setQueueLabel(const std::string &label);
        RenderContextBuilder &setBackend(RenderContextBackend backend);
        RenderContextBuilder &setSurfaceUsage(TextureUsage usage);
        RenderContextBuilder &addSurfaceUsage(TextureUsage usage);
        RenderContextBuilder &setPreferredSurfaceFormat(TextureFormat format);
        RenderContextBuilder &setPreferredPresentMode(PresentMode mode);
        RenderContextBuilder &setPreferredCompositeAlphaMode(CompositeAlphaMode mode);
        RenderContextBuilder &setPowerPreference(PowerPreference preference);
        RenderContextBuilder &setForceFallbackAdapter(bool forceFallbackAdapter);

        std::shared_ptr<RenderContext> build(std::shared_ptr<Window> &window);

       private:
        std::string m_Label;
        std::string m_DeviceLabel;
        std::string m_QueueLabel;
        RenderContextBackend m_Backend;
        TextureUsage m_SurfaceUsage;
        TextureFormat m_PreferredSurfaceFormat;
        PresentMode m_PreferredPresentMode;
        CompositeAlphaMode m_PreferredCompositeAlphaMode;
        PowerPreference m_PowerPreference;
        bool m_ForceFallbackAdapter;

       private:
        static void WGPURequestAdapterCallback(WGPURequestAdapterStatus status, WGPUAdapter adapter, const char *message, void *userdata);
        static void WGPURequestDeviceCallback(WGPURequestDeviceStatus status, WGPUDevice device, const char *message, void *userdata);
    };
}  // namespace Yulduz
