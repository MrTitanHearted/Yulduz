#include <Yulduz/Window.hpp>
#include <YulduzGraphics/Context/CommandEncoder.hpp>
#include <YulduzGraphics/Context/GraphicsContext.hpp>
#include <YulduzGraphics/Context/Texture.hpp>

namespace Yulduz {
    void printWGPURegistryReport(const std::string &prefix, WGPURegistryReport report);
    void printHubReport(const std::string &prefix, WGPUHubReport report);

    GraphicsContext::GraphicsContext(const Settings &settings)
        : m_Instance{settings.Instance},
          m_Adapter{settings.Adapter},
          m_Surface{settings.Surface},
          m_Device{settings.Device},
          m_Queue{wgpuDeviceGetQueue(settings.Device)},
          m_Config{settings.Config},
          m_Window{&settings.Window.get()} {
        assert(settings.Instance != nullptr && "Instance handle cannot be nullptr");
        assert(settings.Surface != nullptr && "Surface handle cannot be nullptr");
        assert(settings.Adapter != nullptr && "Adapter handle cannot be nullptr");
        assert(settings.Device != nullptr && "Device handle cannot be nullptr");

        WGPUSupportedLimits supportedLimits{};
        wgpuDeviceGetLimits(m_Device, &supportedLimits);
        auto [width, height] = m_Window->getSize();

        WGPUAdapterProperties adapterProperties{};
        wgpuAdapterGetProperties(m_Adapter, &adapterProperties);

        WGPUSurfaceCapabilities caps{};
        wgpuSurfaceGetCapabilities(m_Surface, m_Adapter, &caps);

        m_Limits = supportedLimits.limits;
        for (std::size_t i = 0; i < caps.formatCount; i++)
            m_Caps.Formats.insert(static_cast<TextureFormat>(caps.formats[i]));
        for (std::size_t i = 0; i < caps.presentModeCount; i++)
            m_Caps.PresentModes.insert(static_cast<PresentMode>(caps.presentModes[i]));
        for (std::size_t i = 0; i < caps.alphaModeCount; i++)
            m_Caps.AlphaModes.insert(static_cast<CompositeAlphaMode>(caps.alphaModes[i]));
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
        YZDEBUG("\tAdapterType: {}", GetAdapterTypeName(m_AdapterProperties.AdapterType));
        YZDEBUG("\tBackendType: {}", GetBackendTypeName(m_AdapterProperties.BackendType));

        wgpuSurfaceCapabilitiesFreeMembers(caps);
    }

    GraphicsContext::GraphicsContext()
        : m_Instance{nullptr},
          m_Surface{nullptr},
          m_Adapter{nullptr},
          m_Device{nullptr},
          m_Queue{nullptr},
          m_Config{},
          m_Limits{},
          m_Window{nullptr},
          m_Caps{},
          m_AdapterProperties{} {
    }

    GraphicsContext::~GraphicsContext() {
        if (m_Instance) {
            wgpuQueueRelease(m_Queue);
            wgpuDeviceRelease(m_Device);
            wgpuAdapterRelease(m_Adapter);
            wgpuSurfaceRelease(m_Surface);
            wgpuInstanceRelease(m_Instance);
        }
    }

    GraphicsContext::GraphicsContext(const GraphicsContext &other)
        : m_Instance{other.m_Instance},
          m_Adapter{other.m_Adapter},
          m_Surface{other.m_Surface},
          m_Device{other.m_Device},
          m_Queue{other.m_Queue},
          m_Config{other.m_Config},
          m_Limits{other.m_Limits},
          m_Window{other.m_Window},
          m_Caps{other.m_Caps},
          m_AdapterProperties{other.m_AdapterProperties} {
        assert(m_Instance != nullptr && "Instance handle cannot be nullptr");
        wgpuInstanceReference(m_Instance);
        wgpuAdapterReference(m_Adapter);
        wgpuSurfaceReference(m_Surface);
        wgpuDeviceReference(m_Device);
        wgpuQueueReference(m_Queue);
    }

    GraphicsContext::GraphicsContext(GraphicsContext &&other)
        : m_Instance{other.m_Instance},
          m_Adapter{other.m_Adapter},
          m_Surface{other.m_Surface},
          m_Device{other.m_Device},
          m_Queue{other.m_Queue},
          m_Config{other.m_Config},
          m_Limits{other.m_Limits},
          m_Window{other.m_Window},
          m_Caps{other.m_Caps},
          m_AdapterProperties{other.m_AdapterProperties} {
        assert(m_Instance != nullptr && "Instance handle cannot be nullptr");
        other.m_Instance = nullptr;
        other.m_Adapter = nullptr;
        other.m_Surface = nullptr;
        other.m_Device = nullptr;
        other.m_Queue = nullptr;
        other.m_Limits = {};
        other.m_Window = nullptr;
        other.m_Caps = {};
        other.m_AdapterProperties = {};
    }

    GraphicsContext &GraphicsContext::operator=(const GraphicsContext &other) {
        assert(other.m_Instance != nullptr && "Instance handle cannot be nullptr");

        if (&other != this) {
            if (m_Instance) {
                wgpuInstanceRelease(m_Instance);
                wgpuAdapterRelease(m_Adapter);
                wgpuSurfaceRelease(m_Surface);
                wgpuDeviceRelease(m_Device);
                wgpuQueueRelease(m_Queue);
            }

            m_Instance = other.m_Instance;
            m_Adapter = other.m_Adapter;
            m_Surface = other.m_Surface;
            m_Device = other.m_Device;
            m_Queue = other.m_Queue;
            m_Config = other.m_Config;
            m_Limits = other.m_Limits;
            m_Window = other.m_Window;
            m_Caps = other.m_Caps;
            m_AdapterProperties = other.m_AdapterProperties;

            wgpuInstanceReference(m_Instance);
            wgpuAdapterReference(m_Adapter);
            wgpuSurfaceReference(m_Surface);
            wgpuDeviceReference(m_Device);
            wgpuQueueReference(m_Queue);
        }

        return *this;
    }

    GraphicsContext &GraphicsContext::operator=(GraphicsContext &&other) {
        assert(other.m_Instance != nullptr && "Instance handle cannot be nullptr");

        if (&other != this) {
            if (m_Instance) {
                wgpuInstanceRelease(m_Instance);
                wgpuAdapterRelease(m_Adapter);
                wgpuSurfaceRelease(m_Surface);
                wgpuDeviceRelease(m_Device);
                wgpuQueueRelease(m_Queue);
            }

            m_Instance = other.m_Instance;
            m_Adapter = other.m_Adapter;
            m_Surface = other.m_Surface;
            m_Device = other.m_Device;
            m_Queue = other.m_Queue;
            m_Config = other.m_Config;
            m_Limits = other.m_Limits;
            m_Window = other.m_Window;
            m_Caps = other.m_Caps;
            m_AdapterProperties = other.m_AdapterProperties;

            other.m_Instance = nullptr;
            other.m_Adapter = nullptr;
            other.m_Surface = nullptr;
            other.m_Device = nullptr;
            other.m_Queue = nullptr;
            other.m_Limits = {};
            other.m_Window = nullptr;
            other.m_Caps = {};
            other.m_AdapterProperties = {};
        }

        return *this;
    }

    void GraphicsContext::resize(std::uint32_t width, std::uint32_t height) {
        assert(m_Instance != nullptr && "Instance handle cannot be nullptr");

        if (m_Config.width == width && m_Config.height == height) return;
        if (width == 0 || height == 0) {
            YZWARN("Attempted to resize window to zero width or height");
            return;
        }

        m_Config.width = width;
        m_Config.height = height;
        wgpuSurfaceConfigure(m_Surface, &m_Config);
    }

    void GraphicsContext::renderFrame(const RenderFrameOnSurfaceCallback &callback) const {
        assert(m_Instance != nullptr && "Instance handle cannot be nullptr");

        WGPUSurfaceTexture surfaceTexture{};
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

        Texture frame{"Yulduz Surface Texture", surfaceTexture.texture, *this};
        callback(frame);
        wgpuSurfacePresent(m_Surface);
    }

    void GraphicsContext::sumbitCommands(const std::vector<CommandBuffer> &commands) const {
        assert(m_Instance != nullptr && "Instance handle cannot be nullptr");

        wgpuQueueSubmit(m_Queue, commands.size(), (WGPUCommandBuffer *)commands.data());
    }

    void GraphicsContext::printWGPUReport() const {
        assert(m_Instance != nullptr && "Instance handle cannot be nullptr");

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

    WGPUInstance GraphicsContext::getInstance() const {
        assert(m_Instance != nullptr && "Instance handle cannot be nullptr");

        return m_Instance;
    }

    WGPUSurface GraphicsContext::getSurface() const {
        assert(m_Instance != nullptr && "Instance handle cannot be nullptr");

        return m_Surface;
    }

    WGPUAdapter GraphicsContext::getAdapter() const {
        assert(m_Instance != nullptr && "Instance handle cannot be nullptr");

        return m_Adapter;
    }

    WGPUDevice GraphicsContext::getDevice() const {
        assert(m_Instance != nullptr && "Instance handle cannot be nullptr");

        return m_Device;
    }

    WGPUQueue GraphicsContext::getQueue() const {
        assert(m_Instance != nullptr && "Instance handle cannot be nullptr");

        return m_Queue;
    }

    WGPUSurfaceConfiguration GraphicsContext::getConfig() const {
        assert(m_Instance != nullptr && "Instance handle cannot be nullptr");

        return m_Config;
    }

    WGPULimits GraphicsContext::getLimits() const {
        assert(m_Instance != nullptr && "Instance handle cannot be nullptr");

        return m_Limits;
    }

    BackendType GraphicsContext::getBackendType() const {
        assert(m_Instance != nullptr && "Instance handle cannot be nullptr");

        return m_AdapterProperties.BackendType;
    }

    SurfaceCapabilities GraphicsContext::getSurfaceCapabilities() const {
        assert(m_Instance != nullptr && "Instance handle cannot be nullptr");

        return m_Caps;
    }

    AdapterProperties GraphicsContext::getAdapterProperties() const {
        assert(m_Instance != nullptr && "Instance handle cannot be nullptr");

        return m_AdapterProperties;
    }

    PresentMode GraphicsContext::getPresentMode() const {
        assert(m_Instance != nullptr && "Instance handle cannot be nullptr");

        return static_cast<PresentMode>(m_Config.presentMode);
    }

    TextureFormat GraphicsContext::getSurfaceFormat() const {
        assert(m_Instance != nullptr && "Instance handle cannot be nullptr");

        return static_cast<TextureFormat>(m_Config.format);
    }

    const Window &GraphicsContext::getWindow() const {
        assert(m_Instance != nullptr && "Instance handle cannot be nullptr");

        return *m_Window;
    }

    void GraphicsContext::setPresentMode(PresentMode mode) {
        assert(m_Instance != nullptr && "Instance handle cannot be nullptr");

        if (!m_Caps.PresentModes.contains(mode)) {
            YZWARN("Attempted to set invalid present mode: '{}'", GetPresentModeName(mode));
            return;
        }

        m_Config.presentMode = static_cast<WGPUPresentMode>(mode);
        wgpuSurfaceConfigure(m_Surface, &m_Config);
    }

    void GraphicsContext::SetupWGPULogging(WGPULogLevel level) {
        wgpuSetLogCallback(WGPULogCallback, nullptr);
        wgpuSetLogLevel(static_cast<WGPULogLevel>(level));
    }

    void GraphicsContext::WGPULogCallback(WGPULogLevel level, const char *message, void *data) {
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

    GraphicsContextBuilder::GraphicsContextBuilder() {
        m_DeviceLabel = "Yulduz Graphics Device";
        m_QueueLabel = "Yulduz Graphics Queue";
#if defined(YULDUZ_PLATFORM_WINDOWS)
        m_Backend = InstanceBackend::DX12;
#else
        m_Backend = InstanceBackend::Primary;
#endif
        m_PreferredSurfaceFormat = TextureFormat::Undefined;
        m_SurfaceUsage = TextureUsage::RenderAttachment;
        m_PreferredPresentMode = PresentMode::Mailbox;
        m_PreferredCompositeAlphaMode = CompositeAlphaMode::Premultiplied;
        m_PowerPreference = PowerPreference::HighPerformance;
        m_ForceFallbackAdapter = false;
    }

    GraphicsContextBuilder &GraphicsContextBuilder::setDeviceLabel(const std::string &label) {
        m_DeviceLabel = label;
        return *this;
    }

    GraphicsContextBuilder &GraphicsContextBuilder::setQueueLabel(const std::string &label) {
        m_QueueLabel = label;
        return *this;
    }

    GraphicsContextBuilder &GraphicsContextBuilder::setBackend(InstanceBackend backend) {
        m_Backend = backend;
        return *this;
    }

    GraphicsContextBuilder &GraphicsContextBuilder::setSurfaceUsage(TextureUsage usage) {
        m_SurfaceUsage = usage;
        return *this;
    }

    GraphicsContextBuilder &GraphicsContextBuilder::addSurfaceUsage(TextureUsage usage) {
        m_SurfaceUsage |= usage;
        return *this;
    }

    GraphicsContextBuilder &GraphicsContextBuilder::setPreferredSurfaceFormat(TextureFormat format) {
        m_PreferredSurfaceFormat = format;
        return *this;
    }

    GraphicsContextBuilder &GraphicsContextBuilder::setPreferredPresentMode(PresentMode mode) {
        m_PreferredPresentMode = mode;
        return *this;
    }

    GraphicsContextBuilder &GraphicsContextBuilder::setPreferredCompositeAlphaMode(CompositeAlphaMode mode) {
        m_PreferredCompositeAlphaMode = mode;
        return *this;
    }

    GraphicsContextBuilder &GraphicsContextBuilder::setPowerPreference(PowerPreference preference) {
        m_PowerPreference = preference;
        return *this;
    }

    GraphicsContextBuilder &GraphicsContextBuilder::setForceFallbackAdapter(bool forceFallbackAdapter) {
        m_ForceFallbackAdapter = forceFallbackAdapter;
        return *this;
    }

    GraphicsContext GraphicsContextBuilder::build(const Window &window) {
        WGPUFeatureName requiredFeatures[]{static_cast<WGPUFeatureName>(WGPUNativeFeature_TextureAdapterSpecificFormatFeatures)};
        WGPURequiredLimitsExtras requiredLimitsExtras{.chain = WGPUChainedStruct{.sType = static_cast<WGPUSType>(WGPUSType_RequiredLimitsExtras)}};
        WGPURequiredLimits requiredLimits{.nextInChain = &requiredLimitsExtras.chain};
        WGPUInstanceExtras instanceExtras{
            .chain = WGPUChainedStruct{.sType = static_cast<WGPUSType>(WGPUSType_InstanceExtras)},
            .backends = static_cast<WGPUInstanceBackendFlags>(m_Backend),
        };
        WGPUInstanceDescriptor instanceDescriptor{.nextInChain = &instanceExtras.chain};
        WGPUInstance instance = wgpuCreateInstance(&instanceDescriptor);
        WGPUSurface surface = window.getWGPUSurface(instance);
        WGPUAdapter adapter;
        WGPUDevice device;
        WGPURequestAdapterOptions adapterOptions{
            .compatibleSurface = surface,
            .powerPreference = static_cast<WGPUPowerPreference>(m_PowerPreference),
            .forceFallbackAdapter = m_ForceFallbackAdapter,
        };
        WGPUDeviceDescriptor deviceDescriptor{
            .label = m_DeviceLabel.c_str(),
            .requiredFeatureCount = std::size(requiredFeatures),
            .requiredFeatures = requiredFeatures,
            // .requiredLimits = &requiredLimits,
            .defaultQueue = WGPUQueueDescriptor{.label = m_QueueLabel.c_str()},
        };
        wgpuInstanceRequestAdapter(instance, &adapterOptions, WGPURequestAdapterCallback, &adapter);
        wgpuAdapterRequestDevice(adapter, &deviceDescriptor, WGPURequestDeviceCallback, &device);

        auto [width, height] = window.getSize();

        if (m_PreferredSurfaceFormat == TextureFormat::Undefined)
            m_PreferredSurfaceFormat = static_cast<TextureFormat>(wgpuSurfaceGetPreferredFormat(surface, adapter));

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

        return GraphicsContext{GraphicsContext::Settings{
            .Window = std::ref(window),
            .Instance = instance,
            .Surface = surface,
            .Adapter = adapter,
            .Device = device,
            .Config = config,
        }};
    }

    void GraphicsContextBuilder::WGPURequestAdapterCallback(WGPURequestAdapterStatus status, WGPUAdapter adapter, const char *message, void *userdata) {
        if (status != WGPURequestAdapterStatus_Success) {
            YZFATAL("Failed to request adapter: {}", message);
            throw std::runtime_error("Failed to request device: " + std::string(message));
        }

        WGPUAdapter *yz = static_cast<WGPUAdapter *>(userdata);
        *yz = adapter;
    }

    void GraphicsContextBuilder::WGPURequestDeviceCallback(WGPURequestDeviceStatus status, WGPUDevice device, const char *message, void *userdata) {
        if (status != WGPURequestDeviceStatus_Success) {
            YZFATAL("Failed to request device: {}", message);
            throw std::runtime_error("Failed to request device: " + std::string(message));
        }

        WGPUDevice *yz = static_cast<WGPUDevice *>(userdata);
        *yz = device;
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
