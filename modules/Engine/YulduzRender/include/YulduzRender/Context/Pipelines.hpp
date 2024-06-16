#pragma once

#include <Yulduz/Core.hpp>
#include <YulduzRender/Enums.hpp>
#include <YulduzRender/Context/BindGroup.hpp>
#include <YulduzRender/Context/Buffers.hpp>
#include <YulduzRender/Context/Shader.hpp>
#include <YulduzRender/Context/VertexLayout.hpp>

namespace Yulduz {
    class RenderContext;

    class BlendState {
       public:
        BlendState();
        ~BlendState() = default;

        BlendState &setColorBlendComponent(BlendOperation operation, BlendFactor srcFactor, BlendFactor dstFactor);
        BlendState &setAlphaBlendComponent(BlendOperation operation, BlendFactor srcFactor, BlendFactor dstFactor);

        WGPUBlendState get() const;

       private:
        WGPUBlendState m_BlendState;
    };

    class ColorTargetState {
       public:
        ColorTargetState(TextureFormat format);
        ~ColorTargetState() = default;

        ColorTargetState &setBlendState(BlendState blendState);
        ColorTargetState &setColorWriteMask(ColorWriteMask writeMask);

        static WGPUColorTargetState Get(const ColorTargetState &state);

       private:
        TextureFormat m_Format;
        std::optional<WGPUBlendState> m_BlendState;
        ColorWriteMask m_ColorWriteMask;
    };

    class StencilFaceState {
       public:
        StencilFaceState(CompareFunction compareFunction);
        ~StencilFaceState() = default;

        StencilFaceState &setFailOperation(StencilOperation operation);
        StencilFaceState &setDepthFailOperation(StencilOperation operation);
        StencilFaceState &setPassOperation(StencilOperation operation);

        WGPUStencilFaceState get() const;

       private:
        WGPUStencilFaceState m_StencilFaceState;
    };

    class VertexState {
       public:
        VertexState(const std::shared_ptr<Shader> &shader, const std::string &entryPoint = "vs_main");
        ~VertexState() = default;

        VertexState &setVertexLayouts(const std::vector<VertexLayout> &layouts);
        VertexState &addVertexLayouts(const std::vector<VertexLayout> &layouts);
        VertexState &addVertexLayout(const VertexLayout &layout);

        static WGPUVertexState Get(const VertexState &state);

       private:
        std::string m_EntryPoint;
        std::shared_ptr<Shader> m_Shader;
        std::vector<VertexLayout> m_BufferLayouts;
        std::vector<WGPUVertexBufferLayout> m_WGPUVertexBufferLayouts;
    };

    class PrimitiveState {
       public:
        PrimitiveState();
        ~PrimitiveState() = default;

        PrimitiveState &setTopology(PrimitiveTopology topology);
        PrimitiveState &setStripIndexFormat(IndexFormat format);
        PrimitiveState &setFrontFace(FrontFace frontFace);
        PrimitiveState &setCullMode(CullMode cullMode);

        WGPUPrimitiveState get() const;

       private:
        WGPUPrimitiveState m_PrimitiveState;
    };

    class DepthStencilState {
       public:
        DepthStencilState();
        ~DepthStencilState() = default;

        DepthStencilState &setFormat(TextureFormat format);
        DepthStencilState &setDepthWrite(bool enabled);
        DepthStencilState &setDepthCompare(CompareFunction compareFunction);
        DepthStencilState &setDepthBias(int32_t bias);
        DepthStencilState &setDepthBiasSlopeScale(float scale);
        DepthStencilState &setDepthBiasClamp(float clamp);
        DepthStencilState &setStencilFront(StencilFaceState stencilFaceState);
        DepthStencilState &setStencilBack(StencilFaceState stencilFaceState);
        DepthStencilState &setStencilReadMask(uint32_t mask);
        DepthStencilState &setStencilWriteMask(uint32_t mask);

        WGPUDepthStencilState get() const;

       private:
        WGPUDepthStencilState m_DepthStencilState;
    };

    class MultisampleState {
       public:
        MultisampleState();
        ~MultisampleState() = default;

        MultisampleState &setCount(uint32_t count);
        MultisampleState &setMask(uint32_t mask);
        MultisampleState &setAlphaToCoverage(bool enabled);

        WGPUMultisampleState get() const;

       private:
        WGPUMultisampleState m_MultisampleState;
    };

    class FragmentState {
       public:
        FragmentState(const std::shared_ptr<Shader> &shader, const std::string &entryPoint = "fs_main");
        ~FragmentState() = default;

        FragmentState &setColorTargetStates(const std::vector<ColorTargetState> &states);
        FragmentState &addColorTargetStates(const std::vector<ColorTargetState> &states);
        FragmentState &addColorTargetState(const ColorTargetState &state);

        static WGPUFragmentState Get(const FragmentState &state);

       private:
        std::string m_EntryPoint;
        std::shared_ptr<Shader> m_Shader;
        std::vector<ColorTargetState> m_ColorTargetStates;
        std::vector<WGPUColorTargetState> m_WGPUColorTargetStates;
    };

    class PipelineLayout {
       public:
        PipelineLayout(const std::string &label, const WGPUPipelineLayout &layout);
        ~PipelineLayout();

        std::string getLabel() const;
        WGPUPipelineLayout get() const;

       private:
        std::string m_Label;
        WGPUPipelineLayout m_PipelineLayout;
    };

    class RenderPipeline {
       public:
        RenderPipeline(const std::string &label, const WGPURenderPipeline &pipeline, const std::shared_ptr<PipelineLayout> &layout);
        ~RenderPipeline();

        std::string getLabel() const;
        WGPURenderPipeline get() const;
        std::shared_ptr<PipelineLayout> getLayout() const;

       private:
        std::string m_Label;
        WGPURenderPipeline m_RenderPipeline;
        std::shared_ptr<PipelineLayout> m_PipelineLayout;
    };

    class PipelineLayoutBuilder {
       public:
        PipelineLayoutBuilder();
        ~PipelineLayoutBuilder() = default;

        inline static PipelineLayoutBuilder New() { return PipelineLayoutBuilder(); }

        PipelineLayoutBuilder &setLabel(const std::string &label);
        PipelineLayoutBuilder &setBindGroupLayouts(const std::vector<std::shared_ptr<BindGroupLayout>> &layouts);
        PipelineLayoutBuilder &addBindGroupLayouts(const std::vector<std::shared_ptr<BindGroupLayout>> &layouts);
        PipelineLayoutBuilder &addBindGroupLayout(const std::shared_ptr<BindGroupLayout> &layout);

        std::shared_ptr<PipelineLayout> build(const std::shared_ptr<RenderContext> &context);

       private:
        std::string m_Label;
        std::vector<std::shared_ptr<BindGroupLayout>> m_BindGroupLayouts;
    };

    class RenderPipelineBuilder {
       public:
        RenderPipelineBuilder();
        ~RenderPipelineBuilder() = default;

        inline static RenderPipelineBuilder New() { return RenderPipelineBuilder(); }

        RenderPipelineBuilder &setLabel(const std::string &label);
        RenderPipelineBuilder &setPrimtiveState(const PrimitiveState &state);
        RenderPipelineBuilder &setMultisampleState(const MultisampleState &state);
        RenderPipelineBuilder &setVertexState(const VertexState &state);
        RenderPipelineBuilder &setFragmentState(const FragmentState &state);
        RenderPipelineBuilder &setDepthStencilState(const DepthStencilState &state);

        std::shared_ptr<RenderPipeline> build(const std::shared_ptr<PipelineLayout> &layout, const std::shared_ptr<RenderContext> &context);

       private:
        std::string m_Label;
        PrimitiveState m_PrimitiveState;
        MultisampleState m_MultisampleState;
        std::optional<VertexState> m_VertexState;
        std::optional<FragmentState> m_FragmentState;
        std::optional<DepthStencilState> m_DepthStencilState;
    };
}  // namespace Yulduz