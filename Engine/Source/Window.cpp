#include <Yulduz/Window.hpp>

namespace Yulduz {
    Window *Window::s_pInstance = nullptr;

    YULDUZ_IF_DEBUG(Window::State Window::s_State{});

    Window &Window::GetRef() {
        DYULDUZ_ASSERT(nullptr != s_pInstance, "Yulduz::Window is not initialized");
        return *s_pInstance;
    }

    SDL_Window *Window::GetHandle() {
        DYULDUZ_ASSERT(nullptr != s_pInstance, "Yulduz::Window is not initialized");
        return s_pInstance->m_Window;
    }

    void Window::initialize(const Settings &settings) {
        DYULDUZ_ASSERT(nullptr == s_pInstance, "Yulduz::Window is already initialized");

        const SDL_PropertiesID window_properties = SDL_CreateProperties();

        SDL_SetBooleanProperty(window_properties, SDL_PROP_WINDOW_CREATE_FOCUSABLE_BOOLEAN, true);
        SDL_SetBooleanProperty(window_properties, SDL_PROP_WINDOW_CREATE_EXTERNAL_GRAPHICS_CONTEXT_BOOLEAN, true);

        SDL_SetStringProperty(window_properties, SDL_PROP_WINDOW_CREATE_TITLE_STRING, settings.Title.data());
        SDL_SetNumberProperty(window_properties, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, settings.Width);
        SDL_SetNumberProperty(window_properties, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, settings.Width);

        if (settings.Resizable) {
            SDL_SetBooleanProperty(window_properties, SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN, true);
        } else {
            SDL_SetBooleanProperty(window_properties, SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN, false);
        }
        if (settings.Fullscreen) {
            SDL_SetBooleanProperty(window_properties, SDL_PROP_WINDOW_CREATE_FULLSCREEN_BOOLEAN, true);
        } else {
            SDL_SetBooleanProperty(window_properties, SDL_PROP_WINDOW_CREATE_FULLSCREEN_BOOLEAN, false);
        }

        m_Window = SDL_CreateWindowWithProperties(window_properties);

        if (!settings.Icon.empty()) {
            if (auto texture_asset_opt = TextureAsset::LoadFromPath(settings.Icon, TextureAsset::Type::eRGBA8u);
                texture_asset_opt.has_value()) {
                const TextureAsset &texture_asset = texture_asset_opt.value();

                SDL_Surface *icon = SDL_CreateSurfaceFrom(
                    static_cast<glm::i32>(texture_asset.getWidth()),
                    static_cast<glm::i32>(texture_asset.getHeight()),
                    SDL_PIXELFORMAT_ABGR8888,
                    (void *)texture_asset.getDataConst().data(),
                    static_cast<glm::i32>(
                        texture_asset.getWidth() *
                        TextureAsset::GetTexelSize(texture_asset.getType())));

                DYULDUZ_ASSERT(SDL_SetWindowIcon(m_Window, icon), "Failed to set window icon: {}", SDL_GetError());

                SDL_DestroySurface(icon);
            } else {
                DYULDUZ_LOG_ENGINE_WARN("Failed to load icon from path: {}", settings.Icon);
            }
        }

        SDL_DestroyProperties(window_properties);

        s_pInstance = this;

        DYULDUZ_LOG_ENGINE_INFO("Yulduz::Window initialized");
    }

    void Window::release() {
        DYULDUZ_ASSERT(nullptr != s_pInstance, "Yulduz::Window is not initialized");

        SDL_DestroyWindow(m_Window);

        m_Window    = nullptr;
        s_pInstance = nullptr;

        DYULDUZ_LOG_ENGINE_INFO("Yulduz::Window released");
    }

    SDL_Window *Window::getHandle() const {
        DYULDUZ_ASSERT(nullptr != s_pInstance, "Yulduz::Window is not initialized");
        return m_Window;
    }

    YULDUZ_IF_DEBUG(Window::State::~State() {
        SDL_assert(nullptr == s_pInstance && "Forgot to release Yulduz::Window");
    });
}  // namespace Yulduz