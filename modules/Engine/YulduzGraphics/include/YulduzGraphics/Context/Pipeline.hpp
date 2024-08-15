#pragma once

#include <Yulduz/Core.hpp>
#include <YulduzGraphics/Enums.hpp>
#include <YulduzGraphics/Context/BindGroup.hpp>
#include <YulduzGraphics/Context/Buffer.hpp>
#include <YulduzGraphics/Context/Sampler.hpp>
#include <YulduzGraphics/Context/Shader.hpp>
#include <YulduzGraphics/Context/Texture.hpp>

namespace Yulduz {
    class GraphicsContext;

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
}  // namespace Yulduz
