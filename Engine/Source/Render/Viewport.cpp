#include "Yulduz/Engine.hpp"

#include <Yulduz/Render.hpp>

namespace Yulduz {
    Render::Viewport Render::k_InvalidViewport{};

    Render::Viewport Render::AddViewport(const eastl::string_view name, const ViewportInfo info) {
        DYULDUZ_ASSERT(nullptr != s_pInstance, "Yulduz::Render is not initialized");
        return s_pInstance->addViewport(name, info);
    }

    void Render::RemoveViewport(const Viewport handle) {
        DYULDUZ_ASSERT(nullptr != s_pInstance, "Yulduz::Render is not initialized");
        return s_pInstance->removeViewport(handle);
    }

    void Render::ResizeViewport(const Viewport handle, const glm::u32 width, const glm::u32 height) {
        DYULDUZ_ASSERT(nullptr != s_pInstance, "Yulduz::Render is not initialized");
        return s_pInstance->resizeViewport(handle, width, height);
    }

    void Render::SetViewportCamera(const Viewport handle, const ViewportCamera &camera) {
        DYULDUZ_ASSERT(nullptr != s_pInstance, "Yulduz::Render is not initialized");
        return s_pInstance->setViewportCamera(handle, camera);
    }

    SDL_GPUTexture *Render::GetViewportTexture(const Viewport handle) {
        DYULDUZ_ASSERT(nullptr != s_pInstance, "Yulduz::Render is not initialized");
        return s_pInstance->getViewportTexture(handle);
    }

    Render::ViewportInfo Render::GetViewportInfo(const Viewport handle) {
        DYULDUZ_ASSERT(nullptr != s_pInstance, "Yulduz::Render is not initialized");
        return s_pInstance->getViewportInfo(handle);
    }

    Render::ViewportCamera Render::GetViewportCamera(const Viewport handle) {
        DYULDUZ_ASSERT(nullptr != s_pInstance, "Yulduz::Render is not initialized");
        return s_pInstance->getViewportCamera(handle);
    }

    Render::Viewport Render::addViewport(const eastl::string_view name, ViewportInfo info) {
        DYULDUZ_ASSERT(nullptr != s_pInstance, "Yulduz::Render is not initialized");
        const Viewport handle{name};

        info.Usage |= SDL_GPU_TEXTUREUSAGE_COLOR_TARGET;

        const auto properties = SDL_CreateProperties();
        SDL_SetStringProperty(properties, SDL_PROP_GPU_TEXTURE_CREATE_NAME_STRING, name.data());

        SDL_GPUTextureCreateInfo create_info{};
        create_info.format = info.Format;
        create_info.type   = info.Type;
        create_info.usage  = info.Usage;

        create_info.width  = info.Width;
        create_info.height = info.Height;

        create_info.layer_count_or_depth = info.LayerCount;

        create_info.num_levels   = info.LevelCount;
        create_info.sample_count = info.SampleCount;

        create_info.props = properties;

        auto &viewport = m_Viewports[handle];

        viewport.Name   = eastl::string{name};
        viewport.Info   = info;
        viewport.Camera = ViewportCamera{glm::mat4x4{1.0f}, glm::mat4x4{1.0f}, glm::vec3{0.0f}};

        viewport.Target = SDL_CreateGPUTexture(m_Device, &create_info);

        SDL_DestroyProperties(properties);

        m_pRenderGraph->setTexture(TextureAlias{name}, viewport.Target);

        return handle;
    }

    void Render::removeViewport(const Viewport handle) {
        DYULDUZ_ASSERT(nullptr != s_pInstance, "Yulduz::Render is not initialized");
        DYULDUZ_ASSERT(m_Viewports.contains(handle), "Yulduz::Render Viewport with handle '{}' is not added", handle.Hash);

        const auto it = m_Viewports.find(handle);

        m_pRenderGraph->setTexture(TextureAlias{it->second.Name}, nullptr);
        SDL_ReleaseGPUTexture(m_Device, it->second.Target);

        m_Viewports.erase(it);
    }

    void Render::resizeViewport(const Viewport handle, const glm::u32 width, const glm::u32 height) {
        DYULDUZ_ASSERT(nullptr != s_pInstance, "Yulduz::Render is not initialized");
        DYULDUZ_ASSERT(m_Viewports.contains(handle), "Yulduz::Render Viewport with handle '{}' is not added", handle.Hash);

        auto &viewport = m_Viewports[handle];

        viewport.Info.Width  = width;
        viewport.Info.Height = height;

        const auto properties = SDL_CreateProperties();
        SDL_SetStringProperty(properties, SDL_PROP_GPU_TEXTURE_CREATE_NAME_STRING, viewport.Name.data());

        SDL_GPUTextureCreateInfo create_info{};
        create_info.format = viewport.Info.Format;
        create_info.type   = viewport.Info.Type;
        create_info.usage  = viewport.Info.Usage;

        create_info.width  = viewport.Info.Width;
        create_info.height = viewport.Info.Height;

        create_info.layer_count_or_depth = viewport.Info.LayerCount;

        create_info.num_levels   = viewport.Info.LevelCount;
        create_info.sample_count = viewport.Info.SampleCount;

        create_info.props = properties;

        SDL_ReleaseGPUTexture(m_Device, viewport.Target);
        viewport.Target = SDL_CreateGPUTexture(m_Device, &create_info);

        SDL_DestroyProperties(properties);

        m_pRenderGraph->setTexture(TextureAlias{viewport.Name}, viewport.Target);
    }

    void Render::setViewportCamera(const Viewport handle, const ViewportCamera &camera) {
        DYULDUZ_ASSERT(nullptr != s_pInstance, "Yulduz::Render is not initialized");
        DYULDUZ_ASSERT(m_Viewports.contains(handle), "Yulduz::Render Viewport with handle '{}' is not added", handle.Hash);

        auto &viewport = m_Viewports[handle];

        viewport.Camera = camera;
    }

    SDL_GPUTexture *Render::getViewportTexture(Viewport handle) const {
        DYULDUZ_ASSERT(nullptr != s_pInstance, "Yulduz::Render is not initialized");
        DYULDUZ_ASSERT(m_Viewports.contains(handle), "Yulduz::Render Viewport with handle '{}' is not added", handle.Hash);

        const auto &viewport = m_Viewports.at(handle);
        return viewport.Target;
    }

    Render::ViewportInfo Render::getViewportInfo(const Viewport handle) const {
        DYULDUZ_ASSERT(nullptr != s_pInstance, "Yulduz::Render is not initialized");
        DYULDUZ_ASSERT(m_Viewports.contains(handle), "Yulduz::Render Viewport with handle '{}' is not added", handle.Hash);

        const auto &viewport = m_Viewports.at(handle);
        return viewport.Info;
    }

    Render::ViewportCamera Render::getViewportCamera(const Viewport handle) const {
        DYULDUZ_ASSERT(nullptr != s_pInstance, "Yulduz::Render is not initialized");
        DYULDUZ_ASSERT(m_Viewports.contains(handle), "Yulduz::Render Viewport with handle '{}' is not added", handle.Hash);

        const auto &viewport = m_Viewports.at(handle);
        return viewport.Camera;
    }

}  // namespace Yulduz