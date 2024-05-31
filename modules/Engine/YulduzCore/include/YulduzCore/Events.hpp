#pragma once

#include <YulduzCore/YulduzPCH.hpp>

namespace Yulduz {
    class IEvent {
       public:
        virtual ~IEvent() = default;
    };

    class EventDispatcher {
       public:
        EventDispatcher() = default;
        ~EventDispatcher() = default;

        template <typename Event, typename = std::enable_if_t<std::is_base_of<IEvent, Event>::value>>
        void addEvent(Event event) {
            m_Events.emplace_back(std::make_unique<Event>(event));
        }

        template <typename Event, typename = std::enable_if_t<std::is_base_of<IEvent, Event>::value>>
        void setCallback(std::function<void(const Event &)> callback) {
            const std::type_info &eventType = typeid(Event);
            m_CallbacksMap[eventType.hash_code()].clear();
            m_CallbacksMap[eventType.hash_code()].emplace_back([callback](const IEvent *event) {
                callback(*dynamic_cast<const Event *>(event));
            });
        }

        template <typename Event, typename = std::enable_if_t<std::is_base_of<IEvent, Event>::value>>
        void addCallback(std::function<void(const Event &)> callback) {
            const std::type_info &eventType = typeid(Event);
            m_CallbacksMap[eventType.hash_code()].emplace_back([callback](const IEvent *event) {
                callback(*dynamic_cast<const Event *>(event));
            });
        }

        template <typename Event, typename = std::enable_if_t<std::is_base_of<IEvent, Event>::value>>
        std::vector<std::function<void(const IEvent *)>> getRawCallbacks() const {
            const std::type_info &eventType = typeid(Event);
            return m_CallbacksMap[eventType.hash_code()];
        }

        template <typename Event, typename = std::enable_if_t<std::is_base_of<IEvent, Event>::value>>
        std::vector<std::function<void(const Event &)>> getCallbacks() const {
            const std::type_info &eventType = typeid(Event);
            const Callbacks &callbacks = m_CallbacksMap[eventType.hash_code()];
            std::vector<std::function<void(const Event &)>> result(callbacks.size());
            std::transform(callbacks.begin(), callbacks.end(), result.begin(),
                           [](Callback callback) {
                               return [callback](const Event &event) {
                                   callback(*dynamic_cast<const IEvent *>(&event));
                               };
                           });
            return result;
        }

        template <typename Event, typename = std::enable_if_t<std::is_base_of<IEvent, Event>::value>>
        void removeCallback(size_t index) {
            const std::type_info &eventType = typeid(Event);
            Callbacks &callbacks = m_CallbacksMap[eventType.hash_code()];
            if (index < 0 || index >= callbacks.size()) return;
            callbacks.erase(callbacks.begin() + index);
        }

        template <typename Event, typename = std::enable_if_t<std::is_base_of<IEvent, Event>::value>>
        void removeFirstCallback() {
            const std::type_info &eventType = typeid(Event);
            Callbacks &callbacks = m_CallbacksMap[eventType.hash_code()];
            if (callbacks.size() <= 0) return;
            callbacks.erase(callbacks.begin());
        }

        template <typename Event, typename = std::enable_if_t<std::is_base_of<IEvent, Event>::value>>
        void removeLastCallback() {
            const std::type_info &eventType = typeid(Event);
            Callbacks &callbacks = m_CallbacksMap[eventType.hash_code()];
            if (callbacks.size() <= 0) return;
            callbacks.erase(callbacks.end() - 1);
        }

        template <typename Event, typename = std::enable_if_t<std::is_base_of<IEvent, Event>::value>>
        void removeCallbacks() {
            const std::type_info &eventType = typeid(Event);
            m_CallbacksMap[eventType.hash_code()].clear();
        }

        void removeAllCallbacks() {
            m_CallbacksMap.clear();
        }

        void dispatch() {
            for (const std::unique_ptr<IEvent> &event : m_Events) {
                size_t eventHashCode = typeid(*event).hash_code();
                if (m_CallbacksMap.find(eventHashCode) == m_CallbacksMap.end()) continue;
                for (const Callback &callback : m_CallbacksMap[eventHashCode]) {
                    callback(event.get());
                }
            }

            m_Events.clear();
        }

       private:
        using Callback = std::function<void(const IEvent *)>;
        using Callbacks = std::vector<Callback>;

        std::vector<std::unique_ptr<IEvent>> m_Events;
        std::unordered_map<size_t, Callbacks> m_CallbacksMap;
    };
}  // namespace Yulduz
