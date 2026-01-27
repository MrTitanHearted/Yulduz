#pragma once

#include <Yulduz/Engine.hpp>

namespace Yulduz {
    template <typename TLayer, typename... Args>
        requires(std::is_base_of_v<ILayer, TLayer>)
    TLayer *Engine::PushLayer(Args &&...args) {
        DYULDUZ_ASSERT(nullptr != s_pInstance, "Yulduz::Engine is not initialized");
        return s_pInstance->pushLayer<TLayer>(std::forward<Args>(args)...);
    }

    template <typename TLayer>
        requires(std::is_base_of_v<ILayer, TLayer>)
    TLayer *Engine::GetLayer() {
        DYULDUZ_ASSERT(nullptr != s_pInstance, "Yulduz::Engine is not initialized");
        return s_pInstance->getLayer<TLayer>();
    }

    template <typename TLayer, typename... Args>
        requires(std::is_base_of_v<ILayer, TLayer>)
    TLayer *Engine::pushLayer(Args &&...args) {
        DYULDUZ_ASSERT(nullptr != s_pInstance, "Yulduz::Engine is not initialized");
        const std::unique_ptr<ILayer> &layer = m_LayerStack.emplace_back(
            std::make_unique<TLayer>(std::forward<Args>(args)...));
        layer->m_pEngine   = this;
        layer->m_LayerType = typeid(TLayer);
        layer->onAttach();
        return static_cast<TLayer *>(layer.get());
    }

    template <typename TLayer>
        requires(std::is_base_of_v<ILayer, TLayer>)
    TLayer *Engine::getLayer() {
        DYULDUZ_ASSERT(nullptr != s_pInstance, "Yulduz::Engine is not initialized");
        auto it = eastl::find_if(
            eastl::begin(m_LayerStack),
            eastl::end(m_LayerStack),
            [](const std::unique_ptr<ILayer> &layer) {
                return layer->getType() == typeid(TLayer);
            });
        if (it == eastl::end(m_LayerStack))
            return nullptr;
        return it->get();
    }
}  // namespace Yulduz