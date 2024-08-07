#pragma once

#include <Yulduz/Core.hpp>
#include <YulduzRender/Enums.hpp>

namespace Yulduz {
    class CommandEncoder;
    class ComputePipeline;
    class BindGroup;

    class ComputePass {
       public:
        ComputePass(const std::string &label, const WGPUComputePassEncoder &encoder);
        ~ComputePass();

        void setComputePipeline(const std::shared_ptr<ComputePipeline> &pipeline);
        void setBindGroups(const std::vector<std::shared_ptr<BindGroup>> &bindGroups);
        void setBindGroup(std::uint32_t groupIndex, const std::shared_ptr<BindGroup> &bindGroup);

        void dispatchWorkGroups(const std::array<std::uint32_t, 3> &workGroupsCount);
        void finish();

        std::string getLabel() const;
        WGPUComputePassEncoder get() const;

       private:
        std::string m_Label;
        WGPUComputePassEncoder m_ComputePassEncoder;
    };

    class ComputePassBuilder {
       public:
        ComputePassBuilder();
        ~ComputePassBuilder() = default;

        inline static ComputePassBuilder New() { return ComputePassBuilder(); }

        ComputePassBuilder &setLabel(const std::string &label);

        std::shared_ptr<ComputePass> build(const std::shared_ptr<CommandEncoder> &encoder);

       private:
        std::string m_Label;
    };
};  // namespace Yulduz