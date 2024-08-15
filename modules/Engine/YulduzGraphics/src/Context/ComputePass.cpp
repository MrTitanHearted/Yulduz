#include <YulduzGraphics/Context/ComputePass.hpp>
#include <YulduzGraphics/Context/CommandEncoder.hpp>
#include <YulduzGraphics/Context/Pipeline.hpp>
#include <YulduzGraphics/Context/BindGroup.hpp>

namespace Yulduz {
    ComputePass::ComputePass(const std::string &label, const WGPUComputePassEncoder &handle)
        : m_Label{label}, m_ComputePassEncoder{handle} {
        assert(handle != nullptr && "ComputePass handle cannot be nullptr");
    }

    ComputePass::ComputePass()
        : m_Label{""}, m_ComputePassEncoder{nullptr} {}

    ComputePass::~ComputePass() {
        if (m_ComputePassEncoder)
            wgpuComputePassEncoderRelease(m_ComputePassEncoder);
    }

    ComputePass::ComputePass(const ComputePass &other)
        : m_Label{other.m_Label}, m_ComputePassEncoder{other.m_ComputePassEncoder} {
        assert(other.m_ComputePassEncoder != nullptr && "ComputePass handle cannot be nullptr");
        wgpuComputePassEncoderReference(m_ComputePassEncoder);
    }

    ComputePass::ComputePass(ComputePass &&other)
        : m_Label{other.m_Label}, m_ComputePassEncoder{other.m_ComputePassEncoder} {
        assert(other.m_ComputePassEncoder != nullptr && "ComputePass handle cannot be nullptr");
        other.m_ComputePassEncoder = nullptr;
        other.m_Label = "";
    }

    ComputePass &ComputePass::operator=(const ComputePass &other) {
        assert(other.m_ComputePassEncoder != nullptr && "ComputePass handle cannot be nullptr");

        if (&other != this) {
            if (m_ComputePassEncoder) wgpuComputePassEncoderRelease(m_ComputePassEncoder);

            m_ComputePassEncoder = other.m_ComputePassEncoder;
            m_Label = other.m_Label;

            wgpuComputePassEncoderReference(m_ComputePassEncoder);
        }

        return *this;
    }

    ComputePass &ComputePass::operator=(ComputePass &&other) {
        assert(other.m_ComputePassEncoder != nullptr && "ComputePass handle cannot be nullptr");

        if (&other != this) {
            if (m_ComputePassEncoder) wgpuComputePassEncoderRelease(m_ComputePassEncoder);

            m_ComputePassEncoder = other.m_ComputePassEncoder;
            m_Label = other.m_Label;

            other.m_ComputePassEncoder = nullptr;
            other.m_Label = "";
        }

        return *this;
    }

    void ComputePass::setPipeline(const ComputePipeline &pipeline) const {
        assert(m_ComputePassEncoder != nullptr && "ComputePass handle cannot be nullptr");

        wgpuComputePassEncoderSetPipeline(m_ComputePassEncoder, pipeline.get());
    }

    void ComputePass::setBindGroups(const std::vector<BindGroup> &bindGroups) const {
        assert(m_ComputePassEncoder != nullptr && "ComputePass handle cannot be nullptr");

        for (std::uint32_t groupIndex = 0; groupIndex < bindGroups.size(); groupIndex++) {
            wgpuComputePassEncoderSetBindGroup(m_ComputePassEncoder, groupIndex, bindGroups[groupIndex].get(), 0, nullptr);
        }
    }

    void ComputePass::setBindGroup(std::uint32_t groupIndex, const BindGroup &bindGroup) const {
        assert(m_ComputePassEncoder != nullptr && "ComputePass handle cannot be nullptr");

        wgpuComputePassEncoderSetBindGroup(m_ComputePassEncoder, groupIndex, bindGroup.get(), 0, nullptr);
    }

    void ComputePass::dispatch(const std::array<std::uint32_t, 3> &workGroupsCount) const {
        assert(m_ComputePassEncoder != nullptr && "ComputePass handle cannot be nullptr");

        wgpuComputePassEncoderDispatchWorkgroups(m_ComputePassEncoder, workGroupsCount[0], workGroupsCount[1], workGroupsCount[2]);
    }

    void ComputePass::dispatch(std::uint32_t workGroupsCountX, std::uint32_t workGroupsCountY, std::uint32_t workGroupsCountZ) const {
        assert(m_ComputePassEncoder != nullptr && "ComputePass handle cannot be nullptr");

        wgpuComputePassEncoderDispatchWorkgroups(m_ComputePassEncoder, workGroupsCountX, workGroupsCountY, workGroupsCountZ);
    }

    void ComputePass::finish() const {
        assert(m_ComputePassEncoder != nullptr && "ComputePass handle cannot be nullptr");

        wgpuComputePassEncoderEnd(m_ComputePassEncoder);
    }

    std::string ComputePass::getLabel() const {
        assert(m_ComputePassEncoder != nullptr && "ComputePass handle cannot be nullptr");

        return m_Label;
    }

    WGPUComputePassEncoder ComputePass::get() const {
        assert(m_ComputePassEncoder != nullptr && "ComputePass handle cannot be nullptr");

        return m_ComputePassEncoder;
    }

    ComputePassBuilder::ComputePassBuilder() {
        m_Label = "Yulduz Compute Pass";
    }

    ComputePassBuilder &ComputePassBuilder::setLabel(const std::string &label) {
        m_Label = label;
        return *this;
    }

    ComputePass ComputePassBuilder::build(const CommandEncoder &encoder) {
        WGPUComputePassDescriptor descriptor{
            .label = m_Label.c_str(),
        };
        return ComputePass{m_Label, wgpuCommandEncoderBeginComputePass(encoder.get(), &descriptor)};
    }
}  // namespace Yulduz
