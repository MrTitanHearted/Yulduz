#pragma once

#include <Yulduz/Core.hpp>
#include <YulduzGraphics/Enums.hpp>
#include <YulduzGraphics/Context/BindGroup.hpp>
#include <YulduzGraphics/Context/Buffer.hpp>
#include <YulduzGraphics/Context/Sampler.hpp>
#include <YulduzGraphics/Context/Shader.hpp>
#include <YulduzGraphics/Context/Texture.hpp>
#include <YulduzGraphics/Context/VertexLayout.hpp>

namespace Yulduz {
    class GraphicsContext;

    class BlendState {
       public:
        BlendState();
        ~BlendState() = default;

        inline static BlendState New() { return BlendState(); }

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

        inline static ColorTargetState New(TextureFormat format) { return ColorTargetState{format}; }

        ColorTargetState &setColorWriteMask(ColorWriteMask colorWriteMask);
        ColorTargetState &setBlendState(const BlendState &blendState);

        static WGPUColorTargetState Get(const ColorTargetState &state);

       private:
        TextureFormat m_Format;
        std::optional<WGPUBlendState> m_BlendState;
        ColorWriteMask m_ColorWriteMask;
    };

    class StencilFaceState {
       public:
        StencilFaceState(CompareFunction compare);
        ~StencilFaceState() = default;

        inline static StencilFaceState New(CompareFunction compare) { return StencilFaceState{compare}; }

        StencilFaceState &setFailOperation(StencilOperation operation);
        StencilFaceState &setDepthFailOperation(StencilOperation operation);
        StencilFaceState &setPassOperation(StencilOperation operation);

        WGPUStencilFaceState get() const;

       private:
        WGPUStencilFaceState m_StencilFaceState;
    };

    class FragmentState {
       public:
        FragmentState(const Shader &shader, const std::string &entry = "fs_main");
        ~FragmentState() = default;

        inline static FragmentState New(const Shader &shader, const std::string &entry = "fs_main") { return FragmentState{shader, entry}; }

        FragmentState &setColorTargetStates(const std::vector<ColorTargetState> &states);
        FragmentState &addColorTargetState(const ColorTargetState &state);

        static WGPUFragmentState Get(const FragmentState &state);

       private:
        std::string m_EntryPoint;
        Shader m_Shader;
        std::vector<ColorTargetState> m_ColorTargetStates;
        std::vector<WGPUColorTargetState> m_WGPUColorTargetStates;
    };

    class VertexState {
       public:
        VertexState(const Shader &shader, const std::string &entry = "vs_main");
        ~VertexState() = default;

        inline static VertexState New(const Shader &shader, const std::string &entry = "vs_main") { return VertexState{shader, entry}; }

        VertexState &setVertexLayouts(const std::vector<VertexLayout> &vertexLayouts);
        VertexState &addVertexLayout(const VertexLayout &vertexLayout);

        static WGPUVertexState Get(const VertexState &state);

       private:
        std::string m_EntryPoint;
        Shader m_Shader;
        std::vector<VertexLayout> m_BufferLayouts;
        std::vector<WGPUVertexBufferLayout> m_WGPUVertexBufferLayouts;
    };

    class PrimitiveState {
       public:
        PrimitiveState();
        ~PrimitiveState() = default;

        inline static PrimitiveState New() { return PrimitiveState(); }

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
        DepthStencilState(TextureFormat format);
        ~DepthStencilState() = default;

        inline static DepthStencilState New(TextureFormat format) { return DepthStencilState(format); }

        DepthStencilState &setDepthWrite(bool enabled);
        DepthStencilState &setDepthCompare(CompareFunction compareFunction);
        DepthStencilState &setDepthBias(std::int32_t bias);
        DepthStencilState &setDepthBiasSlopeScale(float scale);
        DepthStencilState &setDepthBiasClamp(float clamp);
        DepthStencilState &setStencilFront(StencilFaceState stencilFaceState);
        DepthStencilState &setStencilBack(StencilFaceState stencilFaceState);
        DepthStencilState &setStencilReadMask(std::uint32_t mask);
        DepthStencilState &setStencilWriteMask(std::uint32_t mask);

        WGPUDepthStencilState get() const;

       private:
        WGPUDepthStencilState m_DepthStencilState;
    };

    class MultisampleState {
       public:
        MultisampleState();
        ~MultisampleState() = default;

        MultisampleState &setCount(std::uint32_t count);
        MultisampleState &setMask(std::uint32_t mask);
        MultisampleState &setAlphaToCoverage(bool enabled);

        WGPUMultisampleState get() const;

       private:
        WGPUMultisampleState m_MultisampleState;
    };

    class ComputeState {
       public:
        ComputeState(const Shader &shader, const std::string &label = "cs_main");
        ~ComputeState() = default;

        inline static ComputeState New(const Shader &shader, const std::string &label = "cs_main") { return ComputeState{shader, label}; }

        static WGPUProgrammableStageDescriptor Get(const ComputeState &state);

       private:
        std::string m_EntryPoint;
        Shader m_Shader;
    };

    class PipelineLayout {
       public:
        PipelineLayout(const std::string &label, const WGPUPipelineLayout &handle);

        PipelineLayout();
        ~PipelineLayout();

        PipelineLayout(const PipelineLayout &other);
        PipelineLayout(PipelineLayout &&other);

        PipelineLayout &operator=(const PipelineLayout &other);
        PipelineLayout &operator=(PipelineLayout &&other);

        static PipelineLayout Auto();

        std::string getLabel() const;
        WGPUPipelineLayout get() const;

       private:
        std::string m_Label;
        WGPUPipelineLayout m_PipelineLayout;
        bool m_IsAuto;
    };

    class ComputePipeline {
       public:
        ComputePipeline(const std::string &label, const WGPUComputePipeline &handle, const PipelineLayout &pipelineLayout);

        ComputePipeline();
        ~ComputePipeline();

        ComputePipeline(const ComputePipeline &other);
        ComputePipeline(ComputePipeline &&other);

        ComputePipeline &operator=(const ComputePipeline &other);
        ComputePipeline &operator=(ComputePipeline &&other);

        std::string getLabel() const;
        WGPUComputePipeline get() const;
        const PipelineLayout &getLayoutRef() const;

        BindGroupLayout getBindGroupLayout(std::uint32_t groupIndex) const;

       private:
        std::string m_Label;
        WGPUComputePipeline m_ComputePipeline;

        PipelineLayout m_PipelineLayout;
    };

    class RenderPipeline {
       public:
        RenderPipeline(const std::string &label, const WGPURenderPipeline &handle, const PipelineLayout &pipelineLayout);

        RenderPipeline();
        ~RenderPipeline();

        RenderPipeline(const RenderPipeline &other);
        RenderPipeline(RenderPipeline &&other);

        RenderPipeline &operator=(const RenderPipeline &other);
        RenderPipeline &operator=(RenderPipeline &&other);

        std::string getLabel() const;
        WGPURenderPipeline get() const;
        const PipelineLayout &getLayoutRef() const;

        BindGroupLayout getBindGroupLayout(std::uint32_t groupIndex) const;

       private:
        std::string m_Label;
        WGPURenderPipeline m_RenderPipeline;
        PipelineLayout m_PipelineLayout;
    };

    class PipelineLayoutBuilder {
       public:
        PipelineLayoutBuilder();
        ~PipelineLayoutBuilder() = default;

        inline static PipelineLayoutBuilder New() { return PipelineLayoutBuilder(); }

        PipelineLayoutBuilder &setLabel(const std::string &label);
        PipelineLayoutBuilder &setBindGroupLayouts(const std::vector<BindGroupLayout> &bindGroupLayouts);
        PipelineLayoutBuilder &addBindGroupLayout(const BindGroupLayout &bindGroupLayout);

        PipelineLayout build(const GraphicsContext &context);

       private:
        std::string m_Label;
        std::vector<BindGroupLayout> m_BindGroupLayouts;
    };

    class ComputePipelineBuilder {
       public:
        ComputePipelineBuilder();
        ~ComputePipelineBuilder() = default;

        inline static ComputePipelineBuilder New() { return ComputePipelineBuilder(); }

        ComputePipelineBuilder &setLabel(const std::string &label);
        ComputePipelineBuilder &setComputeStateReq(const ComputeState &computeState);

        ComputePipeline build(const PipelineLayout &layout, const GraphicsContext &context);
        ComputePipeline buildAutoLayout(const GraphicsContext &context);

       private:
        std::string m_Label;
        std::optional<ComputeState> m_ComputeState;
    };

    class RenderPipelineBuilder {
       public:
        RenderPipelineBuilder();
        ~RenderPipelineBuilder() = default;

        inline static RenderPipelineBuilder New() { return RenderPipelineBuilder(); }

        RenderPipelineBuilder &setLabel(const std::string &label);
        RenderPipelineBuilder &setPrimtiveState(const PrimitiveState &state);
        RenderPipelineBuilder &setMultisampleState(const MultisampleState &state);
        RenderPipelineBuilder &setVertexStateReq(const VertexState &state);
        RenderPipelineBuilder &setFragmentStateReq(const FragmentState &state);
        RenderPipelineBuilder &setDepthStencilState(const DepthStencilState &state);

        RenderPipeline build(const PipelineLayout &layout, const GraphicsContext &context);
        RenderPipeline buildAutoLayout(const GraphicsContext &context);

       private:
        std::string m_Label;
        PrimitiveState m_PrimitiveState;
        MultisampleState m_MultisampleState;
        std::optional<VertexState> m_VertexState;
        std::optional<FragmentState> m_FragmentState;
        std::optional<DepthStencilState> m_DepthStencilState;
    };
}  // namespace Yulduz
