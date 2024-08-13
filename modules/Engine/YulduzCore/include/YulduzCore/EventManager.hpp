#pragma once

#include <YulduzCore/YulduzPCH.hpp>

namespace Yulduz {
    class IEvent {
       public:
        virtual ~IEvent() = default;
    };

    class CallbackManager {
       public:
        CallbackManager() = default;
        ~CallbackManager() = default;

        template <typename Event, typename = std::enable_if_t<std::is_base_of<IEvent, Event>::value>>
        void addCallback(void (*callback)(const Event &)) {
            const std::size_t eventHashCode = typeid(Event).hash_code();
            m_CallbacksMap[eventHashCode].push_back([callback](const IEvent *event) {
                (*callback)(*static_cast<const Event *>(event));
            });
        }

        template <typename Event, typename T, typename = std::enable_if_t<std::is_base_of<IEvent, Event>::value>>
        void addCallback(void (T::*callback)(const Event &), T *self) {
            const std::size_t eventHashCode = typeid(Event).hash_code();
            m_CallbacksMap[eventHashCode].push_back([callback, self](const IEvent *event) {
                (self->*callback)(*static_cast<const Event *>(event));
            });
        }

       protected:
        std::unordered_map<std::size_t, std::vector<std::function<void(const IEvent *)>>> m_CallbacksMap;
    };

    class EventDispatcher : public CallbackManager {
       public:
        EventDispatcher() = default;
        ~EventDispatcher() = default;

        template <typename Event, typename = std::enable_if_t<std::is_base_of<IEvent, Event>::value>>
        constexpr void addEvent(const Event &event) {
            const std::size_t eventHashCode = typeid(Event).hash_code();

            if (m_EventDataMap.find(eventHashCode) == m_EventDataMap.end()) {
                m_EventDataMap[eventHashCode] = std::make_unique<EventVector<Event>>();
            }

            std::vector<Event> &eventVector = static_cast<EventVector<Event> &>(*(m_EventDataMap[eventHashCode].get())).m_Events;
            m_EventIndicesMap[eventHashCode].emplace_back(eventVector.size());
            eventVector.emplace_back(event);
        }

        void dispatch() {
            if (m_CallbacksMap.empty() || m_EventIndicesMap.empty()) return;


            if (m_CallbacksMap.size() < m_EventIndicesMap.size())
                for (const auto &[eventHashCode, callbacks] : m_CallbacksMap) {
                    const auto &eventIndices = m_EventIndicesMap[eventHashCode];
                    const auto &ieventVector = m_EventDataMap[eventHashCode];
                    if (eventIndices.empty()) continue;

                    for (const auto &callback : callbacks) {
                        for (const std::uint32_t &eventIndex : eventIndices) {
                            callback(ieventVector->get(eventIndex));
                        }
                    }
                }
            else
                for (const auto &[eventHashCode, eventIndices] : m_EventIndicesMap) {
                    const auto &callbacks = m_CallbacksMap[eventHashCode];
                    const auto &ieventVector = m_EventDataMap[eventHashCode];
                    if (callbacks.empty()) continue;

                    for (const auto &callback : callbacks) {
                        for (const std::uint32_t &eventIndex : eventIndices) {
                            callback(ieventVector->get(eventIndex));
                        }
                    }
                }

            m_EventIndicesMap.clear();
            m_EventDataMap.clear();
        }

        static EventDispatcher &GetDefault() { return g_EventDispatcher; }

       private:
        class IEventVector {
           public:
            virtual ~IEventVector() = default;
            virtual const IEvent *get(std::uint32_t index) const = 0;
        };

        template <typename Event, typename = std::enable_if_t<std::is_base_of<IEvent, Event>::value>>
        class EventVector : public IEventVector {
           public:
            std::vector<Event> m_Events;

            const IEvent *get(std::uint32_t index) const override {
                return &m_Events[index];
            }
        };

       private:
        std::unordered_map<std::size_t, std::unique_ptr<IEventVector>> m_EventDataMap;
        std::unordered_map<std::size_t, std::vector<std::uint32_t>> m_EventIndicesMap;

        static EventDispatcher g_EventDispatcher;
    };

    class EventObserver : public CallbackManager {
       public:
        EventObserver() = default;
        ~EventObserver() = default;

        template <typename Event, typename = std::enable_if_t<std::is_base_of<IEvent, Event>::value>>
        void process(const Event &event) {
            const std::size_t eventHashCode = typeid(Event).hash_code();
            const auto &callbacks = m_CallbacksMap[eventHashCode];

            for (const auto &callback : callbacks) {
                callback(&event);
            }
        }
    };
}  // namespace Yulduz
