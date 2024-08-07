#pragma once

#include <Yulduz/Engine.hpp>
#include <Yulduz/Scene.hpp>

namespace Yulduz {
    class RayTracer {
       public:
        struct Settings {
            std::reference_wrapper<std::shared_ptr<RenderContext>> RenderContext;
        };

        struct Options {
            bool Accumulate;
            std::int32_t Bounces;
            glm::vec3 SkyColor;
            bool AddSky;
        };

       public:
        RayTracer(const Settings &settings);
        ~RayTracer();

        void resize(std::uint32_t width, std::uint32_t height);
        void updateSceneBuffers(const Scene &scene);
        void render(double time, const CameraManager &camera, const std::shared_ptr<ComputePass> &computePass);
        void reset();

        Options &getOptionsRef();
        const std::shared_ptr<Framebuffer> &getFinalImageRef() const;

       private:
        struct CameraBuffer {
            glm::mat4 InverseProjection;
            glm::mat4 InverseView;
            glm::vec3 Position;
            float Time;
            glm::vec3 SkyColor;
            std::uint32_t AddSky;
            std::uint32_t FrameIndex;
            std::uint32_t Bounces;
            glm::vec2 _Padding;
        };

       private:
        std::shared_ptr<RenderContext> m_Context;

        std::shared_ptr<ComputePipeline> m_ComputePipeline;

        std::shared_ptr<BindGroupLayout> m_SceneBindGroupLayout;
        std::shared_ptr<BindGroupLayout> m_FrameBindGroupLayout;
        std::shared_ptr<BindGroup> m_CameraBindGroup;
        std::shared_ptr<BindGroup> m_SceneBindGroup;
        std::shared_ptr<BindGroup> m_FrameBindGroup;

        std::shared_ptr<UniformBuffer> m_CameraBuffer;
        std::shared_ptr<StorageBuffer> m_SceneMaterialBuffer;
        std::shared_ptr<StorageBuffer> m_SceneSphereBuffer;

        std::shared_ptr<Framebuffer> m_AccumulatedFramebuffer;
        std::shared_ptr<Framebuffer> m_Framebuffer;

        std::uint32_t m_FrameIndex;
        Options m_Options;
    };
}  // namespace Yulduz
