#include <YulduzRender/Context/ComputePass.hpp>
#include <YulduzRender/Context.hpp>

namespace Yulduz {
    ComputePass::ComputePass(const std::string &label, const WGPUComputePassEncoder &encoder) {
        YZINFO("Initializing Compute Pass: '{}'", label);

        m_Label = label;
        m_ComputePassEncoder = encoder;
    }

    ComputePass::~ComputePass() {
        YZINFO("Releasing Compute Pass: '{}'", m_Label);
        wgpuComputePassEncoderRelease(m_ComputePassEncoder);
    }

    void ComputePass::setComputePipeline(const std::shared_ptr<ComputePipeline> &pipeline) {
        wgpuComputePassEncoderSetPipeline(m_ComputePassEncoder, pipeline->get());
    }

    void ComputePass::setBindGroups(const std::vector<std::shared_ptr<BindGroup>> &bindGroups) {
        for (std::size_t i = 0; i < bindGroups.size(); i++) {
            wgpuComputePassEncoderSetBindGroup(m_ComputePassEncoder, i, bindGroups[i]->get(), 0, nullptr);
        }
    }

    void ComputePass::setBindGroup(std::uint32_t groupIndex, const std::shared_ptr<BindGroup> &bindGroup) {
        wgpuComputePassEncoderSetBindGroup(m_ComputePassEncoder, groupIndex, bindGroup->get(), 0, nullptr);
    }

    void ComputePass::dispatchWorkGroups(const std::array<std::uint32_t, 3> &workGroupsCount) {
        wgpuComputePassEncoderDispatchWorkgroups(m_ComputePassEncoder, workGroupsCount[0], workGroupsCount[1], workGroupsCount[2]);
    }

    void ComputePass::finish() {
        wgpuComputePassEncoderEnd(m_ComputePassEncoder);
    }

    std::string ComputePass::getLabel() const {
        return m_Label;
    }

    WGPUComputePassEncoder ComputePass::get() const {
        return m_ComputePassEncoder;
    }

    ComputePassBuilder::ComputePassBuilder() {
        m_Label = "Yulduz Compute Pass";
    }

    ComputePassBuilder &ComputePassBuilder::setLabel(const std::string &label) {
        m_Label = label;
        return *this;
    }

    std::shared_ptr<ComputePass> ComputePassBuilder::build(const std::shared_ptr<CommandEncoder> &encoder) {
        WGPUComputePassDescriptor descriptor{.label = m_Label.c_str()};
        WGPUComputePassEncoder computePassEncoder = wgpuCommandEncoderBeginComputePass(encoder->get(), &descriptor);
        return std::make_shared<ComputePass>(m_Label, computePassEncoder);
    }
}  // namespace Yulduz