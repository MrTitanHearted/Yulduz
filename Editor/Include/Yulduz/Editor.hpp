#pragma once

#include <Yulduz/Engine.hpp>

namespace Yulduz {
    class EditorLayer : public ILayer {
       public:
        EditorLayer()           = default;
        ~EditorLayer() override = default;

        void onAttach() override;
        void onDetach() override;

       private:
        void onUpdate(glm::f64 delta_time) override;
        void onRender(RenderGraph &render_graph) override;

       private:
    };
}  // namespace Yulduz