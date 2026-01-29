#include <Yulduz/Engine.hpp>

namespace Yulduz {
    Engine *Engine::s_pInstance = nullptr;

    YULDUZ_IF_DEBUG(Engine::State Engine::s_State{});

    Engine &Engine::GetRef() {
        DYULDUZ_ASSERT(nullptr != s_pInstance, "Yulduz::Engine is not initialized");
        return *s_pInstance;
    }

    bool Engine::IsRunning() {
        DYULDUZ_ASSERT(nullptr != s_pInstance, "Yulduz::Engine is not initialized");
        return s_pInstance->m_IsRunning;
    }

    RenderGraph &Engine::GetRenderGraph() {
        DYULDUZ_ASSERT(nullptr != s_pInstance, "Yulduz::Engine is not initialized");
        return s_pInstance->m_RenderGraph;
    }

    void Engine::initialize(const Settings &settings) {
        DYULDUZ_ASSERT(nullptr == s_pInstance, "Yulduz::Engine is already initialized");

        DYULDUZ_ASSERT(SDL_Init(SDL_INIT_VIDEO), "Failed to initialize SDL");

        m_Dispatcher.addListener<SDL_QuitEvent>(
            [this](const SDL_QuitEvent &event) {
                return onQuit(event);
            },
            typeid(Engine));

        m_IsRunning = false;

        m_RenderGraph.setSwapchainTextureAlias(settings.SwapchainTextureAlias);

        m_Window.initialize(settings.WindowSettings);
        m_Render.initialize(settings.RenderSettings, m_Window.getHandle(), m_RenderGraph);

        s_pInstance = this;

        DYULDUZ_LOG_ENGINE_INFO("Yulduz::Engine initialized");
    }

    void Engine::release() {
        DYULDUZ_ASSERT(nullptr != s_pInstance, "Yulduz::Engine is not initialized");

        for (const auto &layer : std::views::reverse(m_LayerStack)) {
            layer->onDetach();
            m_Dispatcher.removeListener(layer->getType());
        }
        m_LayerStack.clear();

        m_Dispatcher.removeListener(typeid(Engine));

        m_Render.release();
        m_Window.release();

        m_IsRunning = false;

        SDL_Quit();

        s_pInstance = nullptr;

        DYULDUZ_LOG_ENGINE_INFO("Yulduz::Engine released");
    }

    void Engine::run() {
        DYULDUZ_ASSERT(nullptr != s_pInstance, "Yulduz::Engine is not initialized");

        glm::u64 last_counter = SDL_GetPerformanceCounter();

        m_IsRunning = true;
        while (m_IsRunning) {
            const glm::u64 current_counter = SDL_GetPerformanceCounter();
            const glm::u64 frequency       = SDL_GetPerformanceFrequency();

            m_DeltaTime = static_cast<glm::f64>(current_counter - last_counter) /
                          static_cast<glm::f64>(frequency);
            last_counter = current_counter;

            pollEvents();
            m_Dispatcher.processEvents();

            for (const auto &layer : std::views::reverse(m_LayerStack))
                layer->onUpdate(m_DeltaTime);

            for (const auto &layer : std::views::reverse(m_LayerStack))
                layer->onBeginGUI();

            for (const auto &layer : std::views::reverse(m_LayerStack))
                layer->onGUI();

            for (const auto &layer : std::views::reverse(m_LayerStack))
                layer->onEndGUI();

            for (const auto &layer : std::views::reverse(m_LayerStack))
                layer->onRender(m_RenderGraph);

            for (const auto &layer : std::views::reverse(m_LayerStack))
                layer->onRenderGUI(m_RenderGraph);

            SDL_GPUCommandBuffer *command_buffer    = nullptr;
            SDL_GPUTexture       *swapchain_texture = nullptr;

            if (!m_Render.beginFrame(&command_buffer, &swapchain_texture)) {
                SDL_CancelGPUCommandBuffer(command_buffer);
                continue;
            }

            RenderGraph::Executor executor = m_RenderGraph.buildExecutor(swapchain_texture);
            RenderGraph::Execute(command_buffer, eastl::move(executor));

            m_Render.endFrame(command_buffer);
        }
    }

    bool Engine::isRunning() const {
        DYULDUZ_ASSERT(nullptr != s_pInstance, "Yulduz::Engine is not initialized");
        return m_IsRunning;
    }

    RenderGraph &Engine::getRenderGraph() {
        DYULDUZ_ASSERT(nullptr != s_pInstance, "Yulduz::Engine is not initialized");
        return m_RenderGraph;
    }

    void Engine::pollEvents() {
        SDL_Event event{};

        while (SDL_PollEvent(&event)) {
            m_Dispatcher.postEvent(event);
            switch (event.type) {
                // Application events
                case SDL_EVENT_QUIT:
                    m_Dispatcher.postEvent(event.quit);
                    break;

                case SDL_EVENT_TERMINATING:
                case SDL_EVENT_LOW_MEMORY:
                case SDL_EVENT_WILL_ENTER_BACKGROUND:
                case SDL_EVENT_DID_ENTER_BACKGROUND:
                case SDL_EVENT_WILL_ENTER_FOREGROUND:
                case SDL_EVENT_DID_ENTER_FOREGROUND:
                case SDL_EVENT_LOCALE_CHANGED:
                case SDL_EVENT_SYSTEM_THEME_CHANGED:
                    m_Dispatcher.postEvent(event.common);
                    break;

                // Display events
                case SDL_EVENT_DISPLAY_ORIENTATION:
                case SDL_EVENT_DISPLAY_ADDED:
                case SDL_EVENT_DISPLAY_REMOVED:
                case SDL_EVENT_DISPLAY_MOVED:
                case SDL_EVENT_DISPLAY_DESKTOP_MODE_CHANGED:
                case SDL_EVENT_DISPLAY_CURRENT_MODE_CHANGED:
                case SDL_EVENT_DISPLAY_CONTENT_SCALE_CHANGED:
                case SDL_EVENT_DISPLAY_USABLE_BOUNDS_CHANGED:
                    m_Dispatcher.postEvent(event.display);
                    break;

                // Window events
                case SDL_EVENT_WINDOW_SHOWN:
                case SDL_EVENT_WINDOW_HIDDEN:
                case SDL_EVENT_WINDOW_EXPOSED:
                case SDL_EVENT_WINDOW_MOVED:
                case SDL_EVENT_WINDOW_RESIZED:
                case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
                case SDL_EVENT_WINDOW_METAL_VIEW_RESIZED:
                case SDL_EVENT_WINDOW_MINIMIZED:
                case SDL_EVENT_WINDOW_MAXIMIZED:
                case SDL_EVENT_WINDOW_RESTORED:
                case SDL_EVENT_WINDOW_MOUSE_ENTER:
                case SDL_EVENT_WINDOW_MOUSE_LEAVE:
                case SDL_EVENT_WINDOW_FOCUS_GAINED:
                case SDL_EVENT_WINDOW_FOCUS_LOST:
                case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                case SDL_EVENT_WINDOW_HIT_TEST:
                case SDL_EVENT_WINDOW_ICCPROF_CHANGED:
                case SDL_EVENT_WINDOW_DISPLAY_CHANGED:
                case SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED:
                case SDL_EVENT_WINDOW_SAFE_AREA_CHANGED:
                case SDL_EVENT_WINDOW_OCCLUDED:
                case SDL_EVENT_WINDOW_ENTER_FULLSCREEN:
                case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN:
                case SDL_EVENT_WINDOW_DESTROYED:
                case SDL_EVENT_WINDOW_HDR_STATE_CHANGED:
                    m_Dispatcher.postEvent(event.window);
                    break;

                // Keyboard events
                case SDL_EVENT_KEY_DOWN:
                case SDL_EVENT_KEY_UP:
                    m_Dispatcher.postEvent(event.key);
                    break;

                case SDL_EVENT_TEXT_EDITING:
                    m_Dispatcher.postEvent(event.edit);
                    break;

                case SDL_EVENT_TEXT_INPUT:
                    m_Dispatcher.postEvent(event.text);
                    break;

                case SDL_EVENT_KEYMAP_CHANGED:
                case SDL_EVENT_SCREEN_KEYBOARD_SHOWN:
                case SDL_EVENT_SCREEN_KEYBOARD_HIDDEN:
                    m_Dispatcher.postEvent(event.common);
                    break;

                case SDL_EVENT_KEYBOARD_ADDED:
                case SDL_EVENT_KEYBOARD_REMOVED:
                    m_Dispatcher.postEvent(event.kdevice);
                    break;

                case SDL_EVENT_TEXT_EDITING_CANDIDATES:
                    m_Dispatcher.postEvent(event.edit_candidates);
                    break;

                // Mouse events
                case SDL_EVENT_MOUSE_MOTION:
                    m_Dispatcher.postEvent(event.motion);
                    break;

                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                case SDL_EVENT_MOUSE_BUTTON_UP:
                    m_Dispatcher.postEvent(event.button);
                    break;

                case SDL_EVENT_MOUSE_WHEEL:
                    m_Dispatcher.postEvent(event.wheel);
                    break;

                case SDL_EVENT_MOUSE_ADDED:
                case SDL_EVENT_MOUSE_REMOVED:
                    m_Dispatcher.postEvent(event.mdevice);
                    break;

                // Joystick events
                case SDL_EVENT_JOYSTICK_AXIS_MOTION:
                    m_Dispatcher.postEvent(event.jaxis);
                    break;

                case SDL_EVENT_JOYSTICK_BALL_MOTION:
                    m_Dispatcher.postEvent(event.jball);
                    break;

                case SDL_EVENT_JOYSTICK_HAT_MOTION:
                    m_Dispatcher.postEvent(event.jhat);
                    break;

                case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
                case SDL_EVENT_JOYSTICK_BUTTON_UP:
                    m_Dispatcher.postEvent(event.jbutton);
                    break;

                case SDL_EVENT_JOYSTICK_ADDED:
                case SDL_EVENT_JOYSTICK_REMOVED:
                case SDL_EVENT_JOYSTICK_UPDATE_COMPLETE:
                    m_Dispatcher.postEvent(event.jdevice);
                    break;

                case SDL_EVENT_JOYSTICK_BATTERY_UPDATED:
                    m_Dispatcher.postEvent(event.jbattery);
                    break;

                // Gamepad events
                case SDL_EVENT_GAMEPAD_AXIS_MOTION:
                    m_Dispatcher.postEvent(event.gaxis);
                    break;

                case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
                case SDL_EVENT_GAMEPAD_BUTTON_UP:
                    m_Dispatcher.postEvent(event.gbutton);
                    break;

                case SDL_EVENT_GAMEPAD_ADDED:
                case SDL_EVENT_GAMEPAD_REMOVED:
                case SDL_EVENT_GAMEPAD_REMAPPED:
                case SDL_EVENT_GAMEPAD_UPDATE_COMPLETE:
                case SDL_EVENT_GAMEPAD_STEAM_HANDLE_UPDATED:
                    m_Dispatcher.postEvent(event.gdevice);
                    break;

                case SDL_EVENT_GAMEPAD_TOUCHPAD_DOWN:
                case SDL_EVENT_GAMEPAD_TOUCHPAD_MOTION:
                case SDL_EVENT_GAMEPAD_TOUCHPAD_UP:
                    m_Dispatcher.postEvent(event.gtouchpad);
                    break;

                case SDL_EVENT_GAMEPAD_SENSOR_UPDATE:
                    m_Dispatcher.postEvent(event.gsensor);
                    break;

                // Touch events
                case SDL_EVENT_FINGER_DOWN:
                case SDL_EVENT_FINGER_UP:
                case SDL_EVENT_FINGER_MOTION:
                case SDL_EVENT_FINGER_CANCELED:
                    m_Dispatcher.postEvent(event.tfinger);
                    break;

                // Pinch events
                case SDL_EVENT_PINCH_BEGIN:
                case SDL_EVENT_PINCH_UPDATE:
                case SDL_EVENT_PINCH_END:
                    m_Dispatcher.postEvent(event.pinch);
                    break;

                // Clipboard events
                case SDL_EVENT_CLIPBOARD_UPDATE:
                    m_Dispatcher.postEvent(event.clipboard);
                    break;

                // Drag and drop events
                case SDL_EVENT_DROP_FILE:
                case SDL_EVENT_DROP_TEXT:
                case SDL_EVENT_DROP_BEGIN:
                case SDL_EVENT_DROP_COMPLETE:
                case SDL_EVENT_DROP_POSITION:
                    m_Dispatcher.postEvent(event.drop);
                    break;

                // Audio device events
                case SDL_EVENT_AUDIO_DEVICE_ADDED:
                case SDL_EVENT_AUDIO_DEVICE_REMOVED:
                case SDL_EVENT_AUDIO_DEVICE_FORMAT_CHANGED:
                    m_Dispatcher.postEvent(event.adevice);
                    break;

                // Sensor events
                case SDL_EVENT_SENSOR_UPDATE:
                    m_Dispatcher.postEvent(event.sensor);
                    break;

                // Pen events
                case SDL_EVENT_PEN_PROXIMITY_IN:
                case SDL_EVENT_PEN_PROXIMITY_OUT:
                    m_Dispatcher.postEvent(event.pproximity);
                    break;

                case SDL_EVENT_PEN_DOWN:
                case SDL_EVENT_PEN_UP:
                    m_Dispatcher.postEvent(event.ptouch);
                    break;

                case SDL_EVENT_PEN_BUTTON_DOWN:
                case SDL_EVENT_PEN_BUTTON_UP:
                    m_Dispatcher.postEvent(event.pbutton);
                    break;

                case SDL_EVENT_PEN_MOTION:
                    m_Dispatcher.postEvent(event.pmotion);
                    break;

                case SDL_EVENT_PEN_AXIS:
                    m_Dispatcher.postEvent(event.paxis);
                    break;

                // Camera device events
                case SDL_EVENT_CAMERA_DEVICE_ADDED:
                case SDL_EVENT_CAMERA_DEVICE_REMOVED:
                case SDL_EVENT_CAMERA_DEVICE_APPROVED:
                case SDL_EVENT_CAMERA_DEVICE_DENIED:
                    m_Dispatcher.postEvent(event.cdevice);
                    break;

                // Render events
                case SDL_EVENT_RENDER_TARGETS_RESET:
                case SDL_EVENT_RENDER_DEVICE_RESET:
                case SDL_EVENT_RENDER_DEVICE_LOST:
                    m_Dispatcher.postEvent(event.render);
                    break;

                // User events
                case SDL_EVENT_USER:
                    m_Dispatcher.postEvent(event.user);
                    break;

                // Private platform events (usually handled by SDL internally)
                case SDL_EVENT_PRIVATE0:
                case SDL_EVENT_PRIVATE1:
                case SDL_EVENT_PRIVATE2:
                case SDL_EVENT_PRIVATE3:
                    // These are typically for internal platform use
                    // You may want to handle them or ignore them
                    break;

                // Internal SDL events
                case SDL_EVENT_POLL_SENTINEL:
                    // This is an internal SDL event marking the end of a poll cycle
                    // Generally should not be processed by user code
                    break;

                default:
                    // Unknown or custom user event (SDL_EVENT_USER + offset)
                    if (event.type >= SDL_EVENT_USER && event.type < SDL_EVENT_LAST) {
                        m_Dispatcher.postEvent(event.user);
                    }
                    break;
            }
        }
    }

    bool Engine::onQuit(const SDL_QuitEvent &event) {
        m_IsRunning = false;
        return false;
    }

    YULDUZ_IF_DEBUG(Engine::State::~State() {
        SDL_assert(nullptr == s_pInstance && "Forgot to release Yulduz::Engine");
    })
}  // namespace Yulduz