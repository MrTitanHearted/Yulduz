#pragma once

#include <Yulduz/Core.hpp>
#include <YulduzGraphics/Enums.hpp>

namespace Yulduz {
    class CommandEncoder;
    class ComputePipeline;
    class BindGroup;
    class BindGroupLayout;

    class ComputePass {
       public:
        ComputePass(const std::string &label, const WGPUComputePassEncoder &handle);

        ComputePass();
        ~ComputePass();

        ComputePass(const ComputePass &other);
        ComputePass(ComputePass &&other);

        ComputePass &operator=(const ComputePass &other);
        ComputePass &operator=(ComputePass &&other);

        void setPipeline(const ComputePipeline &pipeline) const;
        void setBindGroups(const std::vector<BindGroup> &bindGroups) const;
        void setBindGroup(std::uint32_t groupIndex, const BindGroup &bindGroup) const;

        void dispatch(const std::array<std::uint32_t, 3> &workGroupsCount) const;
        void dispatch(std::uint32_t workGroupsCountX, std::uint32_t workGroupsCountY, std::uint32_t workGroupsCountZ) const;
        void finish() const;

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

        ComputePass build(const CommandEncoder &encoder);

       private:
        std::string m_Label;
    };
}  // namespace Yulduz
