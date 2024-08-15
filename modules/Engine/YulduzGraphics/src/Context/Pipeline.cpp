#include <YulduzGraphics/Context/Pipeline.hpp>
#include <YulduzGraphics/Context/GraphicsContext.hpp>

namespace Yulduz {
    ComputeState::ComputeState(const Shader &shader, const std::string &label) : m_Shader{shader}, m_EntryPoint{label} {}

    WGPUProgrammableStageDescriptor ComputeState::Get(const ComputeState &state) {
        return WGPUProgrammableStageDescriptor{
            .module = state.m_Shader.get(),
            .entryPoint = state.m_EntryPoint.c_str(),
        };
    }

    PipelineLayout::PipelineLayout(const std::string &label, const WGPUPipelineLayout &handle)
        : m_Label{label}, m_PipelineLayout{handle}, m_IsAuto{false} {
        assert(handle != nullptr && "PipelineLayout handle cannot be nullptr");
    }

    PipelineLayout::PipelineLayout()
        : m_Label{}, m_PipelineLayout{nullptr}, m_IsAuto{false} {}

    PipelineLayout::~PipelineLayout() {
        if (!m_IsAuto && m_PipelineLayout)
            wgpuPipelineLayoutRelease(m_PipelineLayout);
    }

    PipelineLayout::PipelineLayout(const PipelineLayout &other)
        : m_Label{other.m_Label}, m_PipelineLayout{other.m_PipelineLayout}, m_IsAuto{other.m_IsAuto} {
        if (m_IsAuto) {
            m_Label = "";
            m_PipelineLayout = nullptr;
            return;
        }

        assert(other.m_PipelineLayout != nullptr && "PipelineLayout handle cannot be nullptr");
        wgpuPipelineLayoutReference(m_PipelineLayout);
    }

    PipelineLayout::PipelineLayout(PipelineLayout &&other)
        : m_Label{other.m_Label}, m_PipelineLayout{other.m_PipelineLayout}, m_IsAuto{other.m_IsAuto} {
        if (m_IsAuto) {
            m_Label = "";
            m_PipelineLayout = nullptr;
            return;
        }

        assert(other.m_PipelineLayout != nullptr && "PipelineLayout handle cannot be nullptr");
        other.m_PipelineLayout = nullptr;
        other.m_Label = "";
        other.m_IsAuto = false;
    }

    PipelineLayout &PipelineLayout::operator=(const PipelineLayout &other) {
        if (other.m_IsAuto) {
            m_Label = "";
            m_PipelineLayout = nullptr;
            m_IsAuto = true;
            return *this;
        }

        assert(other.m_PipelineLayout != nullptr && "PipelineLayout handle cannot be nullptr");

        if (&other != this) {
            if (m_PipelineLayout) wgpuPipelineLayoutRelease(m_PipelineLayout);

            m_PipelineLayout = other.m_PipelineLayout;
            m_Label = other.m_Label;
            m_IsAuto = other.m_IsAuto;

            wgpuPipelineLayoutReference(m_PipelineLayout);
        }

        return *this;
    }

    PipelineLayout &PipelineLayout::operator=(PipelineLayout &&other) {
        if (other.m_IsAuto) {
            m_Label = "";
            m_PipelineLayout = nullptr;
            m_IsAuto = true;
            return *this;
        }

        assert(other.m_PipelineLayout != nullptr && "PipelineLayout handle cannot be nullptr");

        if (&other != this) {
            if (m_PipelineLayout) wgpuPipelineLayoutRelease(m_PipelineLayout);

            m_PipelineLayout = other.m_PipelineLayout;
            m_Label = other.m_Label;
            m_IsAuto = other.m_IsAuto;

            other.m_PipelineLayout = nullptr;
            other.m_Label = "";
            other.m_IsAuto = false;
        }

        return *this;
    }

    PipelineLayout PipelineLayout::Auto() {
        PipelineLayout layout{};
        layout.m_IsAuto = true;
        return layout;
    }

    std::string PipelineLayout::getLabel() const {
        if (m_IsAuto) {
            YZWARN("You are trying to access to an auto-generated PipelineLayout handle. This is not supported.");
            return {};
        }

        assert(m_PipelineLayout != nullptr && "PipelineLayout handle cannot be nullptr");

        return m_Label;
    }

    WGPUPipelineLayout PipelineLayout::get() const {
        if (m_IsAuto) {
            YZWARN("You are trying to access to an auto-generated PipelineLayout handle. This is not supported.");
            return {};
        }

        assert(m_PipelineLayout != nullptr && "PipelineLayout handle cannot be nullptr");

        return m_PipelineLayout;
    }

    ComputePipeline::ComputePipeline(const std::string &label, const WGPUComputePipeline &handle, const PipelineLayout &pipelineLayout)
        : m_Label{label}, m_ComputePipeline{handle}, m_PipelineLayout{pipelineLayout} {
        assert(handle != nullptr && "ComputePipeline handle cannot be nullptr");
    }

    ComputePipeline::ComputePipeline()
        : m_Label{}, m_ComputePipeline{nullptr}, m_PipelineLayout{} {}

    ComputePipeline::~ComputePipeline() {
        if (m_ComputePipeline)
            wgpuComputePipelineRelease(m_ComputePipeline);
    }

    ComputePipeline::ComputePipeline(const ComputePipeline &other)
        : m_Label{other.m_Label}, m_ComputePipeline{other.m_ComputePipeline}, m_PipelineLayout{other.m_PipelineLayout} {
        assert(other.m_ComputePipeline != nullptr && "ComputePipeline handle cannot be nullptr");
        wgpuComputePipelineReference(m_ComputePipeline);
    }

    ComputePipeline::ComputePipeline(ComputePipeline &&other)
        : m_Label{other.m_Label}, m_ComputePipeline{other.m_ComputePipeline}, m_PipelineLayout{std::move(other.m_PipelineLayout)} {
        assert(other.m_ComputePipeline != nullptr && "ComputePipeline handle cannot be nullptr");
        other.m_ComputePipeline = nullptr;
        other.m_Label = "";
    }

    ComputePipeline &ComputePipeline::operator=(const ComputePipeline &other) {
        assert(other.m_ComputePipeline != nullptr && "ComputePipeline handle cannot be nullptr");

        if (&other != this) {
            if (m_ComputePipeline) wgpuComputePipelineRelease(m_ComputePipeline);

            m_ComputePipeline = other.m_ComputePipeline;
            m_Label = other.m_Label;
            m_PipelineLayout = other.m_PipelineLayout;

            wgpuComputePipelineReference(m_ComputePipeline);
        }

        return *this;
    }

    ComputePipeline &ComputePipeline::operator=(ComputePipeline &&other) {
        assert(other.m_ComputePipeline != nullptr && "ComputePipeline handle cannot be nullptr");

        if (&other != this) {
            if (m_ComputePipeline) wgpuComputePipelineRelease(m_ComputePipeline);

            m_ComputePipeline = other.m_ComputePipeline;
            m_Label = other.m_Label;
            m_PipelineLayout = std::move(other.m_PipelineLayout);

            other.m_ComputePipeline = nullptr;
            other.m_Label = "";
        }

        return *this;
    }

    std::string ComputePipeline::getLabel() const {
        assert(m_ComputePipeline != nullptr && "ComputePipeline handle cannot be nullptr");

        return m_Label;
    }

    WGPUComputePipeline ComputePipeline::get() const {
        assert(m_ComputePipeline != nullptr && "ComputePipeline handle cannot be nullptr");

        return m_ComputePipeline;
    }

    const PipelineLayout &ComputePipeline::getLayoutRef() const {
        assert(m_ComputePipeline != nullptr && "ComputePipeline handle cannot be nullptr");

        return m_PipelineLayout;
    }

    BindGroupLayout ComputePipeline::getBindGroupLayout(std::uint32_t groupIndex) const {
        assert(m_ComputePipeline != nullptr && "ComputePipeline handle cannot be nullptr");

        return BindGroupLayout{wgpuComputePipelineGetBindGroupLayout(m_ComputePipeline, groupIndex)};
    }

    PipelineLayoutBuilder::PipelineLayoutBuilder() {
        m_Label = "Yulduz PipelineLayout";
    }

    PipelineLayoutBuilder &PipelineLayoutBuilder::setLabel(const std::string &label) {
        m_Label = label;
        return *this;
    }

    PipelineLayoutBuilder &PipelineLayoutBuilder::setBindGroupLayouts(const std::vector<BindGroupLayout> &bindGroupLayouts) {
        m_BindGroupLayouts = bindGroupLayouts;
        return *this;
    }

    PipelineLayoutBuilder &PipelineLayoutBuilder::addBindGroupLayout(const BindGroupLayout &bindGroupLayout) {
        m_BindGroupLayouts.emplace_back(bindGroupLayout);
        return *this;
    }

    PipelineLayout PipelineLayoutBuilder::build(const GraphicsContext &context) {
        WGPUPipelineLayoutDescriptor descriptor{
            .label = m_Label.c_str(),
            .bindGroupLayoutCount = m_BindGroupLayouts.size(),
            .bindGroupLayouts = (WGPUBindGroupLayout *)m_BindGroupLayouts.data(),
        };
        return PipelineLayout{m_Label, wgpuDeviceCreatePipelineLayout(context.getDevice(), &descriptor)};
    }

    ComputePipelineBuilder::ComputePipelineBuilder() {
        m_Label = "Yulduz ComputePipeline";
    }

    ComputePipelineBuilder &ComputePipelineBuilder::setLabel(const std::string &label) {
        m_Label = label;
        return *this;
    }

    ComputePipelineBuilder &ComputePipelineBuilder::setComputeStateReq(const ComputeState &computeState) {
        m_ComputeState = computeState;
        return *this;
    }

    ComputePipeline ComputePipelineBuilder::build(const PipelineLayout &layout, const GraphicsContext &context) {
        if (!m_ComputeState) {
            YZFATAL("Compute Pipeline: '{}' Compute State is not provided!", m_Label);
            throw std::runtime_error("Compute Pipeline: '" + m_Label + "' Compute State is not provided!");
        }
        WGPUProgrammableStageDescriptor computeState = ComputeState::Get(m_ComputeState.value());
        WGPUComputePipelineDescriptor descriptor{
            .label = m_Label.c_str(),
            .layout = layout.get(),
            .compute = computeState,
        };
        return ComputePipeline{m_Label, wgpuDeviceCreateComputePipeline(context.getDevice(), &descriptor), layout};
    }

    ComputePipeline ComputePipelineBuilder::buildAutoLayout(const GraphicsContext &context) {
        if (!m_ComputeState) {
            YZFATAL("Compute Pipeline: '{}' Compute State is not provided!", m_Label);
            throw std::runtime_error("Compute Pipeline: '" + m_Label + "' Compute State is not provided!");
        }
        WGPUProgrammableStageDescriptor computeState = ComputeState::Get(m_ComputeState.value());
        WGPUComputePipelineDescriptor descriptor{
            .label = m_Label.c_str(),
            .compute = computeState,
        };
        return ComputePipeline{m_Label, wgpuDeviceCreateComputePipeline(context.getDevice(), &descriptor), PipelineLayout::Auto()};
    }
}  // namespace Yulduz
