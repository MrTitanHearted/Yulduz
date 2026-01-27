#pragma once

#include <Yulduz/Common.hpp>
#include <Yulduz/Render.hpp>

namespace Yulduz {
    class YULDUZ_API Engine;

    class YULDUZ_API ILayer {
       public:
        virtual ~ILayer() = default;

       protected:
        virtual void onAttach() = 0;
        virtual void onDetach() = 0;

        virtual void onUpdate(glm::f64 delta_time) {}

        virtual void onRender(RenderGraph &render_graph) {}

        virtual void onGUI() {}

        virtual void onBeginGUI() {}

        virtual void onEndGUI() {}

        virtual void onRenderGUI(RenderGraph &render_graph) {}

        template <typename TEvent>
        void attachCallback(eastl::function<bool(const TEvent &)> callback) const;

        template <typename TEvent>
        void detachCallback() const;

        template <typename TEvent>
        void postEvent(const TEvent &event) const;

        void postTask(eastl::function<void()> task) const;

        std::type_index getType() const;

       protected:
       private:
        Engine *m_pEngine = nullptr;

        std::type_index m_LayerType = typeid(ILayer);

       private:
        friend class Engine;
    };
}  // namespace Yulduz