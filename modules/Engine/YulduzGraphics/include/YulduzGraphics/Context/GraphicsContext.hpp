#pragma once

#include <Yulduz/Core.hpp>
#include <YulduzGraphics/Enums.hpp>

namespace Yulduz {
    class Texture;
    class CommandBuffer;
    class TextureView;
    class Window;

    typedef void (*RenderFrameOnSurfaceCallback)(const Texture &frame);

    struct AdapterProperties {
        std::uint32_t VendorID;
        std::string VendorName;
        std::string Architecture;
        std::uint32_t DeviceID;
        std::string Name;
        std::string DriverDescription;
        AdapterType AdapterType;
        BackendType BackendType;
    };

    struct SurfaceCapabilities {
        std::unordered_set<TextureFormat> Formats;
        std::unordered_set<PresentMode> PresentModes;
        std::unordered_set<CompositeAlphaMode> AlphaModes;
    };

    class GraphicsContext {
       public:
        struct Settings {
            std::reference_wrapper<const Window> Window;
            WGPUInstance Instance;
            WGPUSurface Surface;
            WGPUAdapter Adapter;
            WGPUDevice Device;
            WGPUSurfaceConfiguration Config;
        };

       public:
        GraphicsContext(const Settings &settings);

        GraphicsContext();
        ~GraphicsContext();

        GraphicsContext(const GraphicsContext &other);
        GraphicsContext(GraphicsContext &&other);

        GraphicsContext &operator=(const GraphicsContext &other);
        GraphicsContext &operator=(GraphicsContext &&other);

        void resize(std::uint32_t width, std::uint32_t height);
        void renderFrame(const RenderFrameOnSurfaceCallback &callback) const;
        void sumbitCommands(const std::vector<CommandBuffer> &commands) const;
        void printWGPUReport() const;
        void setPresentMode(PresentMode mode);

        WGPUInstance getInstance() const;
        WGPUSurface getSurface() const;
        WGPUAdapter getAdapter() const;
        WGPUDevice getDevice() const;
        WGPUQueue getQueue() const;
        WGPUSurfaceConfiguration getConfig() const;
        WGPULimits getLimits() const;

        BackendType getBackendType() const;
        SurfaceCapabilities getSurfaceCapabilities() const;
        AdapterProperties getAdapterProperties() const;
        PresentMode getPresentMode() const;
        TextureFormat getSurfaceFormat() const;
        const Window &getWindow() const;

        template <typename T>
        void renderFrame(void (T::*callback)(const Texture &frame), T *self) const;

        static void SetupWGPULogging(WGPULogLevel level);

       private:
        WGPUInstance m_Instance;
        WGPUSurface m_Surface;
        WGPUAdapter m_Adapter;
        WGPUDevice m_Device;
        WGPUQueue m_Queue;
        WGPUSurfaceConfiguration m_Config;
        WGPULimits m_Limits;
        const Window *m_Window;
        SurfaceCapabilities m_Caps;
        AdapterProperties m_AdapterProperties;

       private:
        static void WGPULogCallback(WGPULogLevel level, const char *message, void *data);
    };

    class GraphicsContextBuilder {
       public:
        GraphicsContextBuilder();
        ~GraphicsContextBuilder() = default;

        inline static GraphicsContextBuilder New() { return GraphicsContextBuilder(); }

        GraphicsContextBuilder &setDeviceLabel(const std::string &label);
        GraphicsContextBuilder &setQueueLabel(const std::string &label);
        GraphicsContextBuilder &setBackend(InstanceBackend backend);
        GraphicsContextBuilder &setSurfaceUsage(TextureUsage usage);
        GraphicsContextBuilder &addSurfaceUsage(TextureUsage usage);
        GraphicsContextBuilder &setPreferredSurfaceFormat(TextureFormat format);
        GraphicsContextBuilder &setPreferredPresentMode(PresentMode mode);
        GraphicsContextBuilder &setPreferredCompositeAlphaMode(CompositeAlphaMode mode);
        GraphicsContextBuilder &setPowerPreference(PowerPreference preference);
        GraphicsContextBuilder &setForceFallbackAdapter(bool forceFallbackAdapter);

        GraphicsContext build(const Window &window);

       private:
        std::string m_DeviceLabel;
        std::string m_QueueLabel;
        InstanceBackend m_Backend;
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
