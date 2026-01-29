#include <Yulduz/Render.hpp>

namespace Yulduz {
    Render *Render::s_pInstance = nullptr;

    YULDUZ_IF_DEBUG(Render::State Render::s_State{});

    Render &Render::GetRef() {
        DYULDUZ_ASSERT(nullptr != s_pInstance, "Yulduz::Render is not initialized");
        return *s_pInstance;
    }

    SDL_GPUDevice *Render::GetDevice() {
        DYULDUZ_ASSERT(nullptr != s_pInstance, "Yulduz::Render is not initialized");
        return s_pInstance->m_Device;
    }

    void Render::initialize(const Settings &settings, SDL_Window *window, RenderGraph &render_graph) {
        DYULDUZ_ASSERT(nullptr == s_pInstance, "Yulduz::Render is already initialized");

        m_Window = window;

        const SDL_PropertiesID device_properties = SDL_CreateProperties();

        SDL_SetBooleanProperty(device_properties, SDL_PROP_GPU_DEVICE_CREATE_FEATURE_ANISOTROPY_BOOLEAN, true);

#if defined(YULDUZ_BUILD_TYPE_DEBUG) || defined(YULDUZ_BUILD_TYPE_RELWITHDEBINFO)
        SDL_SetBooleanProperty(device_properties, SDL_PROP_GPU_DEVICE_CREATE_DEBUGMODE_BOOLEAN, true);
        SDL_SetBooleanProperty(device_properties, SDL_PROP_GPU_DEVICE_CREATE_VERBOSE_BOOLEAN, true);
#else
        SDL_SetBooleanProperty(device_properties, SDL_PROP_GPU_DEVICE_CREATE_DEBUGMODE_BOOLEAN, false);
        SDL_SetBooleanProperty(device_properties, SDL_PROP_GPU_DEVICE_CREATE_VERBOSE_BOOLEAN, false);
#endif
        if ((settings.ShaderFormat & SDL_GPU_SHADERFORMAT_PRIVATE) == SDL_GPU_SHADERFORMAT_PRIVATE) {
            SDL_SetBooleanProperty(device_properties, SDL_PROP_GPU_DEVICE_CREATE_SHADERS_PRIVATE_BOOLEAN, true);
        } else {
            SDL_SetBooleanProperty(device_properties, SDL_PROP_GPU_DEVICE_CREATE_SHADERS_PRIVATE_BOOLEAN, false);
        }

        if ((settings.ShaderFormat & SDL_GPU_SHADERFORMAT_SPIRV) == SDL_GPU_SHADERFORMAT_SPIRV) {
            SDL_SetBooleanProperty(device_properties, SDL_PROP_GPU_DEVICE_CREATE_SHADERS_SPIRV_BOOLEAN, true);
        } else {
            SDL_SetBooleanProperty(device_properties, SDL_PROP_GPU_DEVICE_CREATE_SHADERS_SPIRV_BOOLEAN, false);
        }
        if ((settings.ShaderFormat & SDL_GPU_SHADERFORMAT_DXBC) == SDL_GPU_SHADERFORMAT_DXBC) {
            SDL_SetBooleanProperty(device_properties, SDL_PROP_GPU_DEVICE_CREATE_SHADERS_DXBC_BOOLEAN, true);
        } else {
            SDL_SetBooleanProperty(device_properties, SDL_PROP_GPU_DEVICE_CREATE_SHADERS_DXBC_BOOLEAN, false);
        }
        if ((settings.ShaderFormat & SDL_GPU_SHADERFORMAT_DXIL) == SDL_GPU_SHADERFORMAT_DXIL) {
            SDL_SetBooleanProperty(device_properties, SDL_PROP_GPU_DEVICE_CREATE_SHADERS_DXIL_BOOLEAN, true);
        } else {
            SDL_SetBooleanProperty(device_properties, SDL_PROP_GPU_DEVICE_CREATE_SHADERS_DXIL_BOOLEAN, false);
        }
        if ((settings.ShaderFormat & SDL_GPU_SHADERFORMAT_MSL) == SDL_GPU_SHADERFORMAT_MSL) {
            SDL_SetBooleanProperty(device_properties, SDL_PROP_GPU_DEVICE_CREATE_SHADERS_MSL_BOOLEAN, true);
        } else {
            SDL_SetBooleanProperty(device_properties, SDL_PROP_GPU_DEVICE_CREATE_SHADERS_MSL_BOOLEAN, false);
        }
        if ((settings.ShaderFormat & SDL_GPU_SHADERFORMAT_METALLIB) == SDL_GPU_SHADERFORMAT_METALLIB) {
            SDL_SetBooleanProperty(device_properties, SDL_PROP_GPU_DEVICE_CREATE_SHADERS_METALLIB_BOOLEAN, true);
        } else {
            SDL_SetBooleanProperty(device_properties, SDL_PROP_GPU_DEVICE_CREATE_SHADERS_METALLIB_BOOLEAN, false);
        }

        m_Device = SDL_CreateGPUDeviceWithProperties(device_properties);
        DYULDUZ_ASSERT(nullptr != m_Device, "Failed to create GPU device: {}", SDL_GetError());

        {
            const SDL_PropertiesID properties = SDL_GetGPUDeviceProperties(m_Device);

            DYULDUZ_LOG_ENGINE_INFO("Yulduz::Render Device Name: {}", SDL_GetStringProperty(properties, SDL_PROP_GPU_DEVICE_NAME_STRING, nullptr));
            DYULDUZ_LOG_ENGINE_INFO("Yulduz::Render Device Driver Name: {}", SDL_GetStringProperty(properties, SDL_PROP_GPU_DEVICE_DRIVER_NAME_STRING, nullptr));
            DYULDUZ_LOG_ENGINE_INFO("Yulduz::Render Device Driver Version: {}", SDL_GetStringProperty(properties, SDL_PROP_GPU_DEVICE_DRIVER_VERSION_STRING, nullptr));
            DYULDUZ_LOG_ENGINE_INFO("Yulduz::Render Device Driver Info: {}", SDL_GetStringProperty(properties, SDL_PROP_GPU_DEVICE_DRIVER_INFO_STRING, nullptr));
        }

        SDL_DestroyProperties(device_properties);

        DYULDUZ_ASSERT(
            SDL_ClaimWindowForGPUDevice(m_Device, m_Window),
            "Failed to claim window for device: {}",
            SDL_GetError());

        DYULDUZ_ASSERT(
            SDL_SetGPUSwapchainParameters(m_Device, m_Window, settings.SwapchainComposition, settings.PresentMode),
            "Failed to set the swapchain parameters: {}",
            SDL_GetError());

        SDL_SetGPUAllowedFramesInFlight(m_Device, settings.MaxFramesInFlight);

        m_pRenderGraph = &render_graph;

        s_pInstance = this;

        DYULDUZ_LOG_ENGINE_INFO("Yulduz::Render initialized");
    }

    void Render::release() {
        DYULDUZ_ASSERT(nullptr != s_pInstance, "Yulduz::Render is not initialized");

        for (const auto &[_, viewport] : m_Viewports) {
            SDL_ReleaseGPUTexture(m_Device, viewport.Target);
        }

        m_Viewports.clear();

        SDL_ReleaseWindowFromGPUDevice(m_Device, m_Window);
        SDL_DestroyGPUDevice(m_Device);

        m_Device    = nullptr;
        m_Window    = nullptr;
        s_pInstance = nullptr;

        DYULDUZ_LOG_ENGINE_INFO("Yulduz::Render released");
    }

    SDL_GPUDevice *Render::getDevice() const {
        DYULDUZ_ASSERT(nullptr != s_pInstance, "Yulduz::Render is not initialized");
        return m_Device;
    }

    bool Render::beginFrame(SDL_GPUCommandBuffer **command_buffer, SDL_GPUTexture **swapchain_texture) const {
        *command_buffer = SDL_AcquireGPUCommandBuffer(m_Device);

        DYULDUZ_ASSERT(
            nullptr != *command_buffer,
            "Failed to acquire command buffer: {}",
            SDL_GetError());

        SDL_WaitAndAcquireGPUSwapchainTexture(*command_buffer, m_Window, swapchain_texture, nullptr, nullptr);

        return nullptr != *swapchain_texture;
    }

    void Render::endFrame(SDL_GPUCommandBuffer *command_buffer) const {
        SDL_SubmitGPUCommandBuffer(command_buffer);
    }

    YULDUZ_IF_DEBUG(Render::State::~State() {
        SDL_assert(nullptr == s_pInstance && "Forgot to release Yulduz::Render");
    })
}  // namespace Yulduz