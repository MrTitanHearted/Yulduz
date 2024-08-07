#include <YulduzRender/Context.hpp>

namespace Yulduz {
    void printWGPURegistryReport(const std::string &prefix, WGPURegistryReport report);
    void printHubReport(const std::string &prefix, WGPUHubReport report);

    RenderContext::RenderContext(const std::shared_ptr<Window> &window,
                                 const std::string &label,
                                 WGPUInstance instance,
                                 WGPUSurface surface,
                                 WGPUAdapter adapter,
                                 WGPUDevice device,
                                 WGPUSurfaceConfiguration config) {
        YZDEBUG("Initializing Render Context: '{}'", label);
        WGPUSupportedLimits supportedLimits{};
        wgpuDeviceGetLimits(device, &supportedLimits);
        auto [width, height] = window->getSize();

        WGPUAdapterProperties adapterProperties{};
        wgpuAdapterGetProperties(adapter, &adapterProperties);

        m_Label = label;
        m_Instance = instance;
        m_Surface = surface;
        m_Adapter = adapter;
        m_Device = device;
        m_Queue = wgpuDeviceGetQueue(device);
        m_Config = config;
        m_Limits = supportedLimits.limits;
        m_Window = window;
        WGPUSurfaceCapabilities caps{};
        wgpuSurfaceGetCapabilities(surface, adapter, &caps);
        for (std::size_t i = 0; i < caps.formatCount; i++)
            m_Caps.Formats.insert(static_cast<TextureFormat>(caps.formats[i]));
        for (std::size_t i = 0; i < caps.presentModeCount; i++)
            m_Caps.PresentModes.insert(static_cast<PresentMode>(caps.presentModes[i]));
        for (std::size_t i = 0; i < caps.alphaModeCount; i++)
            m_Caps.AlphaModes.insert(static_cast<CompositeAlphaMode>(caps.alphaModes[i]));
        wgpuSurfaceCapabilitiesFreeMembers(caps);
        m_AdapterProperties = AdapterProperties{
            .VendorID = adapterProperties.vendorID,
            .VendorName = adapterProperties.vendorName,
            .Architecture = adapterProperties.architecture,
            .DeviceID = adapterProperties.deviceID,
            .Name = adapterProperties.name,
            .DriverDescription = adapterProperties.driverDescription,
            .AdapterType = static_cast<AdapterType>(adapterProperties.adapterType),
            .BackendType = static_cast<BackendType>(adapterProperties.backendType),
        };

        YZDEBUG("Adapter Properties:");
        YZDEBUG("\tVendorID: {}", m_AdapterProperties.VendorID);
        YZDEBUG("\tVendorName: {}", m_AdapterProperties.VendorName);
        YZDEBUG("\tArchitecture: {}", m_AdapterProperties.Architecture);
        YZDEBUG("\tDeviceID: {}", m_AdapterProperties.DeviceID);
        YZDEBUG("\tName: {}", m_AdapterProperties.Name);
        YZDEBUG("\tDriverDescription: {}", m_AdapterProperties.DriverDescription);
        YZDEBUG("\tAdapterType: {}", GetAdapterType(m_AdapterProperties.AdapterType));
        YZDEBUG("\tBackendType: {}", GetBackendType(m_AdapterProperties.BackendType));
    }

    RenderContext::~RenderContext() {
        YZDEBUG("Releasing Render Context: '{}'", m_Label);

        wgpuQueueRelease(m_Queue);
        wgpuDeviceRelease(m_Device);
        wgpuAdapterRelease(m_Adapter);
        wgpuSurfaceRelease(m_Surface);
        wgpuInstanceRelease(m_Instance);
    }

    void RenderContext::resize(std::uint32_t width, std::uint32_t height) {
        if (width == 0 || height == 0 || (m_Config.width == width && m_Config.height == height)) return;

        m_Config.width = width;
        m_Config.height = height;
        wgpuSurfaceConfigure(m_Surface, &m_Config);
    }

    void RenderContext::renderFrameOnSurface(std::function<void(const std::shared_ptr<Framebuffer> &textrue)> frameCallback) {
        WGPUSurfaceTexture surfaceTexture;
        wgpuSurfaceGetCurrentTexture(m_Surface, &surfaceTexture);
        switch (surfaceTexture.status) {
            case WGPUSurfaceGetCurrentTextureStatus_Success:
                break;
            case WGPUSurfaceGetCurrentTextureStatus_Timeout:
                YZERROR("Failed to get Surface Texture: Timeout");
            case WGPUSurfaceGetCurrentTextureStatus_Outdated:
                YZERROR("Failed to get Surface Texture: Outdated");
            case WGPUSurfaceGetCurrentTextureStatus_Lost:
                YZERROR("Failed to get Surface Texture: Lost");
            case WGPUSurfaceGetCurrentTextureStatus_OutOfMemory:
                YZERROR("Failed to get Surface Texture: Out of memory");
            case WGPUSurfaceGetCurrentTextureStatus_DeviceLost:
                YZERROR("Failed to get Surface Texture: Device lost");
            case WGPUSurfaceGetCurrentTextureStatus_Force32:
                YZERROR("Failed to get Surface Texture");
                return;
        }

        std::shared_ptr<Framebuffer> frame = std::make_shared<Framebuffer>("Yulduz Surface Texture", surfaceTexture.texture);
        frameCallback(frame);
        wgpuSurfacePresent(m_Surface);
    }

    void RenderContext::submitCommands(const std::vector<std::shared_ptr<CommandBuffer>> &commands) const {
        YZINFO("Submitting Commands");
        std::vector<WGPUCommandBuffer> wgpuCommands;
        wgpuCommands.reserve(commands.size());
        for (const std::shared_ptr<CommandBuffer> &commandBuffer : commands) {
            wgpuCommands.emplace_back(commandBuffer->get());
        }
        wgpuQueueSubmit(m_Queue, wgpuCommands.size(), wgpuCommands.data());
    }

    void RenderContext::printWGPUReport() const {
        WGPUGlobalReport report{};
        wgpuGenerateReport(m_Instance, &report);

        printWGPURegistryReport("Surfaces", report.surfaces);

        switch (report.backendType) {
            case WGPUBackendType_WebGPU:
                YZWARN("WebGPU backend is not supported");
                break;
            case WGPUBackendType_D3D11:
                YZWARN("D3D11 backend is not supported");
                break;
            case WGPUBackendType_D3D12:
                printHubReport("Dx12", report.dx12);
                break;
            case WGPUBackendType_Metal:
                printHubReport("Metal", report.metal);
                break;
            case WGPUBackendType_Vulkan:
                printHubReport("Vulkan", report.vulkan);
                break;
            case WGPUBackendType_OpenGL:
            case WGPUBackendType_OpenGLES:
                printHubReport("OpenGL", report.gl);
                break;
            default:
                YZERROR("Unknown backend type: {}", static_cast<std::uint32_t>(report.backendType));
                return;
        }
    }

    void RenderContext::setPresentMode(PresentMode mode) {
        if (!m_Caps.PresentModes.contains(mode)) {
            YZWARN("Yulduz Surface does not support Present Mode: '{}'", static_cast<std::uint32_t>(mode));
            return;
        }

        m_Config.presentMode = static_cast<WGPUPresentMode>(mode);
        wgpuSurfaceConfigure(m_Surface, &m_Config);
    }

    void RenderContext::SetupWGPULogging(WebGPULogLevel level) {
        wgpuSetLogCallback(WGPULogCallback, nullptr);
        wgpuSetLogLevel(static_cast<WGPULogLevel>(level));
    }

    void RenderContext::windowResizeCallback(const WindowResizeEvent &event) {
        resize(event.width, event.height);
    }

    void RenderContext::WGPULogCallback(WGPULogLevel level, const char *message, void *data) {
        switch (level) {
            case WGPULogLevel_Error:
                YZERROR("WebGPU Log: {}", message);
                break;
            case WGPULogLevel_Warn:
                YZWARN("WebGPU Log: {}", message);
                break;
            case WGPULogLevel_Info:
                YZINFO("WebGPU Log: {}", message);
                break;
            case WGPULogLevel_Debug:
                YZDEBUG("WebGPU Log: {}", message);
                break;
            case WGPULogLevel_Trace:
                YZINFO("WebGPU Log: {}", message);
                break;
            default:
                YZWARN("Unknown WebGPU Log: {}", message);
        }
    }

    RenderContextBuilder::RenderContextBuilder() {
        m_Label = "Yulduz Render Context";
        m_DeviceLabel = "Yulduz Device";
        m_QueueLabel = "Yulduz Queue";
#ifdef YULDUZ_PLATFORM_WINDOWS
        m_Backend = RenderContextBackend::DX12;
        m_PreferredSurfaceFormat = TextureFormat::RGBA8Unorm;
#else
        m_Backend = RenderContextBackend::Primary;
        m_PreferredSurfaceFormat = TextureFormat::RGBA8UnormSrgb;
#endif
        m_SurfaceUsage = TextureUsage::RenderAttachment;
        m_PreferredPresentMode = PresentMode::Mailbox;
        m_PreferredCompositeAlphaMode = CompositeAlphaMode::Premultiplied;
        m_PowerPreference = PowerPreference::HighPerformance;
        m_ForceFallbackAdapter = false;
    }

    RenderContextBuilder &RenderContextBuilder::setLabel(const std::string &label) {
        m_Label = label;
        return *this;
    }

    RenderContextBuilder &RenderContextBuilder::setDeviceLabel(const std::string &label) {
        m_DeviceLabel = label;
        return *this;
    }

    RenderContextBuilder &RenderContextBuilder::setQueueLabel(const std::string &label) {
        m_QueueLabel = label;
        return *this;
    }

    RenderContextBuilder &RenderContextBuilder::setBackend(RenderContextBackend backend) {
        m_Backend = backend;
        return *this;
    }

    RenderContextBuilder &RenderContextBuilder::setSurfaceUsage(TextureUsage usage) {
        m_SurfaceUsage = usage;
        return *this;
    }

    RenderContextBuilder &RenderContextBuilder::addSurfaceUsage(TextureUsage usage) {
        m_SurfaceUsage |= usage;
        return *this;
    }

    RenderContextBuilder &RenderContextBuilder::setPreferredSurfaceFormat(TextureFormat format) {
        m_PreferredSurfaceFormat = format;
        return *this;
    }

    RenderContextBuilder &RenderContextBuilder::setPreferredPresentMode(PresentMode mode) {
        m_PreferredPresentMode = mode;
        return *this;
    }

    RenderContextBuilder &RenderContextBuilder::setPreferredCompositeAlphaMode(CompositeAlphaMode mode) {
        m_PreferredCompositeAlphaMode = mode;
        return *this;
    }

    RenderContextBuilder &RenderContextBuilder::setPowerPreference(PowerPreference preference) {
        m_PowerPreference = preference;
        return *this;
    }

    RenderContextBuilder &RenderContextBuilder::setForceFallbackAdapter(bool forceFallbackAdapter) {
        m_ForceFallbackAdapter = forceFallbackAdapter;
        return *this;
    }

    void RenderContextBuilder::WGPURequestAdapterCallback(WGPURequestAdapterStatus status, WGPUAdapter adapter, const char *message, void *userdata) {
        if (status != WGPURequestAdapterStatus_Success) {
            YZERROR("Failed to request adapter: {}", message);
            throw std::runtime_error(std::vformat("Failed to request adapter: {}", std::make_format_args(message)));
        }

        WGPUAdapter *yz = static_cast<WGPUAdapter *>(userdata);
        *yz = adapter;
    }

    void RenderContextBuilder::WGPURequestDeviceCallback(WGPURequestDeviceStatus status, WGPUDevice device, const char *message, void *userdata) {
        if (status != WGPURequestDeviceStatus_Success) {
            YZERROR("Failed to request device: {}", message);
            throw std::runtime_error(std::vformat("Failed to request device: {}", std::make_format_args(message)));
        }

        WGPUDevice *yz = static_cast<WGPUDevice *>(userdata);
        *yz = device;
    }

    std::shared_ptr<RenderContext> RenderContextBuilder::build(std::shared_ptr<Window> &window) {
        WGPUInstanceExtras instanceExtras{
            .chain = WGPUChainedStruct{.sType = static_cast<WGPUSType>(WGPUSType_InstanceExtras)},
            .backends = static_cast<WGPUInstanceBackendFlags>(m_Backend),
        };
        WGPUInstanceDescriptor instanceDescriptor{.nextInChain = &instanceExtras.chain};
        WGPUInstance instance = wgpuCreateInstance(&instanceDescriptor);
        WGPUSurface surface = window->getWGPUSurface(instance);
        WGPUAdapter adapter;
        WGPUDevice device;
        WGPURequestAdapterOptions adapterOptions{
            .compatibleSurface = surface,
            .powerPreference = static_cast<WGPUPowerPreference>(m_PowerPreference),
            .forceFallbackAdapter = m_ForceFallbackAdapter,
        };
        WGPUFeatureName feature = static_cast<WGPUFeatureName>(WGPUNativeFeature_TextureAdapterSpecificFormatFeatures);
        WGPUDeviceDescriptor deviceDescriptor{
            .label = m_DeviceLabel.c_str(),
            .requiredFeatureCount = 1,
            .requiredFeatures = &feature,
            .defaultQueue = WGPUQueueDescriptor{.label = m_QueueLabel.c_str()},
        };
        wgpuInstanceRequestAdapter(instance, &adapterOptions, WGPURequestAdapterCallback, &adapter);
        wgpuAdapterRequestDevice(adapter, &deviceDescriptor, WGPURequestDeviceCallback, &device);

        auto [width, height] = window->getSize();

        WGPUSurfaceCapabilities caps{};
        wgpuSurfaceGetCapabilities(surface, adapter, &caps);
        WGPUTextureFormat surfaceFormat = caps.formats[0];
        WGPUCompositeAlphaMode compositeAlphaMode = caps.alphaModes[0];
        WGPUPresentMode presentMode = caps.presentModes[0];
        for (std::uint32_t i = 0; i < caps.formatCount; i++) {
            if (caps.formats[i] == static_cast<WGPUTextureFormat>(m_PreferredSurfaceFormat)) {
                surfaceFormat = caps.formats[i];
                break;
            }
        }
        for (std::uint32_t i = 0; i < caps.alphaModeCount; i++) {
            if (caps.alphaModes[i] == static_cast<WGPUCompositeAlphaMode>(m_PreferredCompositeAlphaMode)) {
                compositeAlphaMode = caps.alphaModes[i];
                break;
            }
        }
        for (std::uint32_t i = 0; i < caps.presentModeCount; i++) {
            if (caps.presentModes[i] == static_cast<WGPUPresentMode>(m_PreferredPresentMode)) {
                presentMode = caps.presentModes[i];
                break;
            }
        }
        WGPUSurfaceConfiguration config{
            .device = device,
            .format = surfaceFormat,
            .usage = WGPUTextureUsage_RenderAttachment | static_cast<WGPUTextureUsageFlags>(m_SurfaceUsage),
            .viewFormatCount = 0,
            .viewFormats = nullptr,
            .alphaMode = compositeAlphaMode,
            .width = width,
            .height = height,
            .presentMode = presentMode,
        };
        wgpuSurfaceConfigure(surface, &config);
        wgpuSurfaceCapabilitiesFreeMembers(caps);

        return std::make_shared<RenderContext>(window, m_Label, instance, surface, adapter, device, config);
    }

    void printWGPURegistryReport(const std::string &prefix, WGPURegistryReport report) {
        YZDEBUG("Begin Report: {}", prefix);
        YZDEBUG("{}.numAllocated = {}", prefix, report.numAllocated);
        YZDEBUG("{}.numKeptFromUser = {}", prefix, report.numKeptFromUser);
        YZDEBUG("{}.numReleasedFromUser = {}", prefix, report.numReleasedFromUser);
        YZDEBUG("{}.numError = {}", prefix, report.numError);
        YZDEBUG("{}.elementSize = {}", prefix, report.elementSize);
        YZDEBUG("End Report: {}\n", prefix);
    }

    void printHubReport(const std::string &prefix, WGPUHubReport report) {
        printWGPURegistryReport(std::vformat("{}.adapters", std::make_format_args(prefix)), report.adapters);
        printWGPURegistryReport(std::vformat("{}.devices", std::make_format_args(prefix)), report.devices);
        printWGPURegistryReport(std::vformat("{}.queues", std::make_format_args(prefix)), report.queues);
        printWGPURegistryReport(std::vformat("{}.pipelineLayouts", std::make_format_args(prefix)), report.pipelineLayouts);
        printWGPURegistryReport(std::vformat("{}.shaderModules", std::make_format_args(prefix)), report.shaderModules);
        printWGPURegistryReport(std::vformat("{}.bindGroupLayouts", std::make_format_args(prefix)), report.bindGroupLayouts);
        printWGPURegistryReport(std::vformat("{}.bindGroups", std::make_format_args(prefix)), report.bindGroups);
        printWGPURegistryReport(std::vformat("{}.commandBuffers", std::make_format_args(prefix)), report.commandBuffers);
        printWGPURegistryReport(std::vformat("{}.renderBundles", std::make_format_args(prefix)), report.renderBundles);
        printWGPURegistryReport(std::vformat("{}.renderPipelines", std::make_format_args(prefix)), report.renderPipelines);
        printWGPURegistryReport(std::vformat("{}.computePipelines", std::make_format_args(prefix)), report.computePipelines);
        printWGPURegistryReport(std::vformat("{}.querySets", std::make_format_args(prefix)), report.querySets);
        printWGPURegistryReport(std::vformat("{}.buffers", std::make_format_args(prefix)), report.buffers);
        printWGPURegistryReport(std::vformat("{}.textures", std::make_format_args(prefix)), report.textures);
        printWGPURegistryReport(std::vformat("{}.textureViews", std::make_format_args(prefix)), report.textureViews);
        printWGPURegistryReport(std::vformat("{}.samplers", std::make_format_args(prefix)), report.samplers);
    }
}  // namespace Yulduz
