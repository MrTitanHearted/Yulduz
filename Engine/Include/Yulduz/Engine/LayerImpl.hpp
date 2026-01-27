#pragma once

#include <Yulduz/Engine/Layer.hpp>

#include <Yulduz/Engine.hpp>

namespace Yulduz {
    template <typename TEvent>
    void ILayer::attachCallback(eastl::function<bool(const TEvent &)> callback) const {
        m_pEngine->m_Dispatcher.addListener<TEvent>(callback, m_LayerType);
    }

    template <typename TEvent>
    void ILayer::detachCallback() const {
        m_pEngine->m_Dispatcher.removeListener<TEvent>(m_LayerType);
    }

    template <typename TEvent>
    void ILayer::postEvent(const TEvent &event) const {
        m_pEngine->m_Dispatcher.postEvent(event);
    }

    inline void ILayer::postTask(eastl::function<void()> task) const {
        m_pEngine->m_Dispatcher.postTask(std::move(task));
    }

    inline std::type_index ILayer::getType() const {
        return m_LayerType;
    }
}  // namespace Yulduz