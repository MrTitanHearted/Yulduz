#pragma once

#include <Yulduz/Common/API.hpp>
#include <Yulduz/Common/Logger.hpp>

template <>
struct YULDUZ_API eastl::hash<std::type_index> {
    size_t operator()(const std::type_index type) const {
        return std::hash<std::type_index>{}(type);
    }
};

namespace Yulduz {
    template <typename TEvent>
    class EventListener {
       public:
        EventListener(
            eastl::function<bool(const TEvent &)> callback,
            const std::type_index                 listener)
            : m_Callback(callback),
              m_Listener(listener) {}

        ~EventListener() = default;

        bool operator()(const TEvent &event) const {
            return m_Callback(event);
        }

        std::type_index getListener() const {
            return m_Listener;
        }

       private:
        eastl::function<bool(const TEvent &)> m_Callback;

        std::type_index m_Listener;
    };

    class IEventRegistry {
       public:
        virtual ~IEventRegistry() = default;

        virtual void processEvents() = 0;

        virtual void removeListener(std::type_index listener) = 0;
    };

    template <typename TEvent>
    class EventRegistry final : public IEventRegistry {
       public:
        EventRegistry() = default;

        ~EventRegistry() override = default;

        void processEvents() override {
            while (!m_Events.empty()) {
                const TEvent event = m_Events.back();
                m_Events.pop_back();

                bool is_handled = false;
                for (glm::i32 i = m_Listeners.size() - 1; i >= 0 && !is_handled; --i) {
                    is_handled |= m_Listeners[i](event);
                }
            }
        }

        void addListener(
            eastl::function<bool(const TEvent &)> callback,

            const std::type_index listener) {
            m_Listeners.emplace_back(std::move(callback), listener);
        }

        void removeListener(const std::type_index listener) override {
            eastl::remove_if(
                eastl::begin(m_Listeners),
                eastl::end(m_Listeners),
                [&listener](const EventListener<TEvent> &event_listener) {
                    return listener == event_listener.getListener();
                });
        }

        void postEvent(const TEvent &event) {
            m_Events.emplace_back(event);
        }

       private:
        eastl::vector<EventListener<TEvent>> m_Listeners;

        eastl::vector<TEvent> m_Events;
    };

    class YULDUZ_API EventDispatcher {
       public:
        EventDispatcher()  = default;
        ~EventDispatcher() = default;

        template <typename TEvent>
        void addListener(
            eastl::function<bool(const TEvent &)> callback,

            const std::type_index listener) {
            getRegistry<TEvent>().addListener(std::move(callback), listener);
            m_Listeners[listener].emplace(typeid(TEvent));
        }

        template <typename TEvent>
        void removeListener(const std::type_index listener) {
            getRegistry<TEvent>().removeListener(listener);
            m_Listeners.erase(listener);
        }

        void removeListener(const std::type_index listener) {
            for (const auto &event_type : m_Listeners[listener]) {
                m_Registries[event_type]->removeListener(listener);
            }
            m_Listeners.erase(listener);
        }

        template <typename TEvent>
        void postEvent(const TEvent &event) {
            getRegistry<TEvent>()
                .postEvent(event);
        }

        void postTask(eastl::function<void()> &&task) {
            m_TaskQueue.emplace_back(std::move(task));
        }

        void processEvents() {
            for (const auto &[_, registry] : m_Registries) {
                registry->processEvents();
            }

            for (const auto &task : m_TaskQueue) {
                task();
            }

            m_TaskQueue.clear();
        }

       private:
        template <typename TEvent>
        EventRegistry<TEvent> &getRegistry() {
            const std::type_index event_type = typeid(TEvent);

            if (const auto it = m_Registries.find(event_type);
                it != eastl::end(m_Registries)) {
                return *dynamic_cast<EventRegistry<TEvent> *>(it->second.get());
            }

            m_Registries.emplace(event_type, std::make_unique<EventRegistry<TEvent>>());
            return *dynamic_cast<EventRegistry<TEvent> *>(m_Registries[event_type].get());
        }

       private:
        eastl::hash_map<std::type_index, std::unique_ptr<IEventRegistry>>  m_Registries;
        eastl::hash_map<std::type_index, eastl::hash_set<std::type_index>> m_Listeners;

        eastl::vector<eastl::function<void()>> m_TaskQueue;
    };
}  // namespace Yulduz