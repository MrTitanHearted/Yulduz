#pragma once

#include <Yulduz/Engine.hpp>
#include <Yulduz/RayTracedCamera.hpp>
#include <Yulduz/Ray.hpp>
#include <Yulduz/Scene.hpp>

namespace Yulduz {
    class Renderer {
       public:
        struct Settings {
            bool Accumulate = true;
        };

       public:
        Renderer() : m_ActiveScene{nullptr}, m_ActiveCamera{nullptr} {}
        ~Renderer() = default;

        void setRenderContext(const std::shared_ptr<RenderContext> &context) {
            m_Context = context;
        }

        void resize(std::uint32_t width, std::uint32_t height);
        void render(const Scene &scene, const RayTracedCamera &camera);
        void resetFrameIndex() { m_FrameIndex = 1; }

        std::shared_ptr<Framebuffer> getFinalImage() const { return m_FinalImage; }
        Settings &getSettings() { return m_Settings; }

       private:
        struct HitPayload {
            float HitDistance;

            glm::vec3 WorldPosition;
            glm::vec3 WorldNormal;

            std::int32_t ObjectIndex;
        };

       private:
        std::shared_ptr<RenderContext> m_Context;
        std::shared_ptr<Framebuffer> m_FinalImage;
        std::vector<std::uint32_t> m_ImageData;
        std::vector<glm::vec4> m_AccumulationData;
        std::uint32_t m_FrameIndex = 1;

        Settings m_Settings;

        std::vector<std::uint32_t> m_ImageHorizontalIter;
        std::vector<std::uint32_t> m_ImageVerticalIter;

        const Scene *m_ActiveScene;
        const RayTracedCamera *m_ActiveCamera;

       private:
        glm::vec4 perPixel(std::uint32_t x, std::uint32_t y);  // RayGen
        HitPayload traceRay(const Ray &ray);
        HitPayload closestHit(const Ray &ray, float hitDistance, std::int32_t objectIndex);
        HitPayload miss(const Ray &ray);
    };
};  // namespace Yulduz