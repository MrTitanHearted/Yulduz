#include <Yulduz/RayTracer.hpp>

namespace Yulduz {
    RayTracer::RayTracer(const Settings &settings) : m_Context{settings.RenderContext} {
        YZDEBUG("Initializing Ray Tracer");

        auto fileOption = FileAsset::FromPath(fs::path(YULDUZ_SOURCE_DIR) / fs::path("assets") / fs::path("shaders") / fs::path("RayTracing.wgsl"));
        if (!fileOption) throw std::runtime_error("Failed to load a shader file");
        std::shared_ptr<FileAsset> shaderFile = fileOption.value();

        m_FrameIndex = 1;
        m_Options.Accumulate = true;
        m_Options.Bounces = 5;
        m_Options.AddSky = false;
        m_Options.SkyColor = glm::vec3{0.6, 0.7, 0.9};
        auto computeShader = ShaderBuilder::New()
                                 .setLabel("Ray Tracer Compute Shader")
                                 .buildWGSL(shaderFile, m_Context);
        auto cameraBindGroupLayout = BindGroupLayoutBuilder::New()
                                         .setLabel("Ray Tracer Camera Bind Group Layout")
                                         .addUniformBuffer(0)
                                         .build(m_Context);
        m_SceneBindGroupLayout = BindGroupLayoutBuilder::New()
                                     .setLabel("Ray Tracer Scene Bind Group Layout")
                                     .addStorageBuffer(0, BufferEntry(ShaderStage::Compute))
                                     .addStorageBuffer(1, BufferEntry(ShaderStage::Compute))
                                     .build(m_Context);
        m_FrameBindGroupLayout = BindGroupLayoutBuilder::New()
                                     .setLabel("Ray Tracer Frame Bind Group Layout")
                                     .addStorageTexture2D(0, StorageTextureEntry(ShaderStage::Compute, TextureFormat::RGBA32Float)
                                                                 .setStorageTextureAccess(StorageTextureAccess::ReadWrite))
                                     .addStorageTexture2D(1, StorageTextureEntry(ShaderStage::Compute, TextureFormat::RGBA8Unorm))
                                     .build(m_Context);
        auto pipelineLayout = PipelineLayoutBuilder::New()
                                  .setLabel("Ray Tracer Compute Pipeline Layout")
                                  .setBindGroupLayouts({cameraBindGroupLayout, m_SceneBindGroupLayout, m_FrameBindGroupLayout})
                                  .build(m_Context);
        m_ComputePipeline = ComputePipelineBuilder::New()
                                .setLabel("Ray Tracer Compute Pipeline")
                                .setComputeStateReq(ComputeState(computeShader))
                                .build(pipelineLayout, m_Context);

        m_CameraBuffer = UniformBufferBuilder::New()
                             .setLabel("Ray Tracer Camera Uniform Buffer")
                             .empty(sizeof(CameraBuffer), m_Context);
        m_CameraBindGroup = BindGroupBuilder::New()
                                .setLabel("Ray Tracer Camera Bind Group")
                                .addUniformBuffer(0, m_CameraBuffer)
                                .build(cameraBindGroupLayout, m_Context);

        m_SceneMaterialBuffer = StorageBufferBuilder::New()
                                    .setLabel("Ray Tracer Scene Material Storage Buffer")
                                    .empty(sizeof(Material), m_Context);
        m_SceneSphereBuffer = StorageBufferBuilder::New()
                                  .setLabel("Ray Tracer Scene Sphere Storage Buffer")
                                  .empty(sizeof(Sphere), m_Context);
        m_SceneBindGroup = BindGroupBuilder::New()
                               .setLabel("Ray Tracer Scene Bind Group")
                               .addStorageBuffer(0, m_SceneMaterialBuffer)
                               .addStorageBuffer(1, m_SceneSphereBuffer)
                               .build(m_SceneBindGroupLayout, m_Context);

        m_AccumulatedFramebuffer = TextureBuilder::New()
                                       .setLabel("Ray Tracer Accumulation Texture")
                                       .setTextureUsage(TextureUsage::CopyDst | TextureUsage::StorageBinding)
                                       .setFormat(TextureFormat::RGBA32Float)
                                       .emptyFramebuffer(1, 1, m_Context);
        m_Framebuffer = TextureBuilder::New()
                            .setLabel("Ray Tracer Frame texture")
                            .setTextureUsage(TextureUsage::StorageBinding | TextureUsage::CopySrc | TextureUsage::TextureBinding)
                            .setFormat(TextureFormat::RGBA8Unorm)
                            .emptyFramebuffer(1, 1, m_Context);
        m_FrameBindGroup = BindGroupBuilder::New()
                               .setLabel("Ray Tracer Frame Bind Group")
                               .addTexture(0, m_AccumulatedFramebuffer)
                               .addTexture(1, m_Framebuffer)
                               .build(m_FrameBindGroupLayout, m_Context);
    }

    RayTracer::~RayTracer() {
        YZDEBUG("Releasing Ray Tracer");
    }

    void RayTracer::resize(std::uint32_t width, std::uint32_t height) {
        auto [prevWidth, prevHeight] = m_Framebuffer->getSize2D();
        if (prevWidth == width && prevHeight == height) return;
        LOG_INFO("Resizing from {}x{} to {}x{}", prevWidth, prevHeight, width, height);

        m_FrameIndex = 1;
        m_AccumulatedFramebuffer->resize2D(width, height, m_Context);
        m_Framebuffer->resize2D(width, height, m_Context);
        m_FrameBindGroup = BindGroupBuilder::New()
                               .setLabel("Ray Tracer Frame Bind Group")
                               .addTexture(0, m_AccumulatedFramebuffer)
                               .addTexture(1, m_Framebuffer)
                               .build(m_FrameBindGroupLayout, m_Context);
    }

    void RayTracer::updateSceneBuffers(const Scene &scene) {
        bool update = false;
        if (scene.Materials.size() * sizeof(Material) != m_SceneMaterialBuffer->getSize()) {
            m_SceneMaterialBuffer = StorageBufferBuilder::New()
                                        .setLabel("Ray Tracer Scene Material Storage Buffer")
                                        .build(scene.Materials.data(), scene.Materials.size() * sizeof(Material), m_Context);
            update = true;
        }
        if (scene.Spheres.size() * sizeof(Sphere) != m_SceneSphereBuffer->getSize()) {
            m_SceneSphereBuffer = StorageBufferBuilder::New()
                                      .setLabel("Ray Tracer Scene Sphere Storage Buffer")
                                      .build(scene.Spheres.data(), scene.Spheres.size() * sizeof(Sphere), m_Context);
            update = true;
        }

        m_SceneMaterialBuffer->write((void *)scene.Materials.data(), m_Context);
        m_SceneSphereBuffer->write((void *)scene.Spheres.data(), m_Context);

        if (!update) return;

        m_SceneBindGroup = BindGroupBuilder::New()
                               .setLabel("Ray Tracer Scene Bind Group")
                               .addStorageBuffer(0, m_SceneMaterialBuffer)
                               .addStorageBuffer(1, m_SceneSphereBuffer)
                               .build(m_SceneBindGroupLayout, m_Context);
    }

    void RayTracer::render(double time, const CameraManager &camera, const std::shared_ptr<ComputePass> &computePass) {
        auto [width, height] = m_Framebuffer->getSize2D();
        CameraBuffer cameraBuffer{
            .InverseProjection = camera.getInverseProjection(),
            .InverseView = camera.getInverseView(),
            .Position = camera.getPosition(),
            .Time = static_cast<float>(time),
            .SkyColor = m_Options.SkyColor,
            .AddSky = m_Options.AddSky ? 1u : 0u,
            .FrameIndex = m_FrameIndex,
            .Bounces = static_cast<std::uint32_t>(m_Options.Bounces),
        };
        m_CameraBuffer->write(&cameraBuffer, m_Context);

        computePass->setComputePipeline(m_ComputePipeline);
        computePass->setBindGroups({m_CameraBindGroup, m_SceneBindGroup, m_FrameBindGroup});
        computePass->dispatchWorkGroups({(width + 15) / 16, (height + 15) / 16, 1});

        if (m_Options.Accumulate)
            m_FrameIndex++;
        else
            m_FrameIndex = 1;
    }

    void RayTracer::reset() {
        m_FrameIndex = 1;
    }

    RayTracer::Options &RayTracer::getOptionsRef() {
        return m_Options;
    }

    const std::shared_ptr<Framebuffer> &RayTracer::getFinalImage() const {
        return m_Framebuffer;
    }
}  // namespace Yulduz
