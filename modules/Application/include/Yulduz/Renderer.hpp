#pragma once

#include <Yulduz/Engine.hpp>
#include <Yulduz/RayTracedCamera.hpp>
#include <Yulduz/Ray.hpp>
#include <Yulduz/Scene.hpp>

namespace Yulduz {
    class Renderer {
       public:
        Renderer() = default;
        ~Renderer() = default;

        void setRenderContext(const std::shared_ptr<RenderContext> &context) {
            m_Context = context;
        }

        void resize(std::uint32_t width, std::uint32_t height);
        void render(const Scene &scene, const RayTracedCamera &camera);

        std::shared_ptr<Framebuffer> getFinalImage() const { return m_FinalImage; }

       private:
        std::shared_ptr<RenderContext> m_Context;
        std::shared_ptr<Framebuffer> m_FinalImage;
        std::vector<std::uint32_t> m_ImageData;

       private:
        glm::vec4 traceRay(const Scene &scene, const Ray &ray);
    };
};  // namespace Yulduz