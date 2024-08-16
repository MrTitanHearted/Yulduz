#include <YulduzGraphics/Context/Pipeline.hpp>
#include <YulduzGraphics/Context/GraphicsContext.hpp>

namespace Yulduz {
    BlendState::BlendState() {
        m_BlendState = WGPUBlendState{
            .color = WGPUBlendComponent{
                .operation = WGPUBlendOperation_Subtract,
                .srcFactor = WGPUBlendFactor_One,
                .dstFactor = WGPUBlendFactor_One,
            },
            .alpha = WGPUBlendComponent{
                .operation = WGPUBlendOperation_Subtract,
                .srcFactor = WGPUBlendFactor_One,
                .dstFactor = WGPUBlendFactor_One,
            },
        };
    }

    BlendState &BlendState::setColorBlendComponent(BlendOperation operation, BlendFactor srcFactor, BlendFactor dstFactor) {
        m_BlendState.color = WGPUBlendComponent{
            .operation = static_cast<WGPUBlendOperation>(operation),
            .srcFactor = static_cast<WGPUBlendFactor>(srcFactor),
            .dstFactor = static_cast<WGPUBlendFactor>(dstFactor),
        };
        return *this;
    }

    BlendState &BlendState::setAlphaBlendComponent(BlendOperation operation, BlendFactor srcFactor, BlendFactor dstFactor) {
        m_BlendState.alpha = WGPUBlendComponent{
            .operation = static_cast<WGPUBlendOperation>(operation),
            .srcFactor = static_cast<WGPUBlendFactor>(srcFactor),
            .dstFactor = static_cast<WGPUBlendFactor>(dstFactor),
        };
        return *this;
    }

    WGPUBlendState BlendState::get() const {
        return m_BlendState;
    }

    ColorTargetState::ColorTargetState(TextureFormat format) {
        m_Format = format;
        m_BlendState = std::nullopt;
        m_ColorWriteMask = ColorWriteMask::All;
    }

    ColorTargetState &ColorTargetState::setColorWriteMask(ColorWriteMask writeMask) {
        m_ColorWriteMask = writeMask;
        return *this;
    }

    ColorTargetState &ColorTargetState::setBlendState(const BlendState &blendState) {
        m_BlendState = blendState.get();
        return *this;
    }

    WGPUColorTargetState ColorTargetState::Get(const ColorTargetState &state) {
        return WGPUColorTargetState{
            .format = static_cast<WGPUTextureFormat>(state.m_Format),
            .blend = state.m_BlendState.has_value() ? &state.m_BlendState.value() : nullptr,
            .writeMask = static_cast<WGPUColorWriteMaskFlags>(state.m_ColorWriteMask),
        };
    }

    StencilFaceState::StencilFaceState(CompareFunction compareFunction) {
        m_StencilFaceState = WGPUStencilFaceState{
            .compare = static_cast<WGPUCompareFunction>(compareFunction),
            .failOp = WGPUStencilOperation_Keep,
            .depthFailOp = WGPUStencilOperation_Keep,
            .passOp = WGPUStencilOperation_Keep,
        };
    }

    StencilFaceState &StencilFaceState::setFailOperation(StencilOperation operation) {
        m_StencilFaceState.failOp = static_cast<WGPUStencilOperation>(operation);
        return *this;
    }

    StencilFaceState &StencilFaceState::setDepthFailOperation(StencilOperation operation) {
        m_StencilFaceState.depthFailOp = static_cast<WGPUStencilOperation>(operation);
        return *this;
    }

    StencilFaceState &StencilFaceState::setPassOperation(StencilOperation operation) {
        m_StencilFaceState.passOp = static_cast<WGPUStencilOperation>(operation);
        return *this;
    }

    WGPUStencilFaceState StencilFaceState::get() const {
        return m_StencilFaceState;
    }

    FragmentState::FragmentState(const Shader &shader, const std::string &entryPoint) {
        m_EntryPoint = entryPoint;
        m_Shader = shader;
    }

    FragmentState &FragmentState::setColorTargetStates(const std::vector<ColorTargetState> &states) {
        m_ColorTargetStates = states;
        m_WGPUColorTargetStates.resize(m_ColorTargetStates.size());
        for (std::size_t i = 0; i < m_ColorTargetStates.size(); i++) {
            m_WGPUColorTargetStates[i] = ColorTargetState::Get(m_ColorTargetStates[i]);
        }
        return *this;
    }

    FragmentState &FragmentState::addColorTargetState(const ColorTargetState &state) {
        m_ColorTargetStates.emplace_back(state);
        m_WGPUColorTargetStates.emplace_back(ColorTargetState::Get(m_ColorTargetStates.back()));
        return *this;
    }

    WGPUFragmentState FragmentState::Get(const FragmentState &state) {
        return WGPUFragmentState{
            .module = state.m_Shader.get(),
            .entryPoint = state.m_EntryPoint.c_str(),
            .targetCount = state.m_WGPUColorTargetStates.size(),
            .targets = state.m_WGPUColorTargetStates.data(),
        };
    }

    VertexState::VertexState(const Shader &shader, const std::string &entryPoint) {
        m_EntryPoint = entryPoint;
        m_Shader = shader;
    }

    VertexState &VertexState::setVertexLayouts(const std::vector<VertexLayout> &layouts) {
        m_BufferLayouts = layouts;
        m_WGPUVertexBufferLayouts.resize(m_BufferLayouts.size());
        for (std::size_t i = 0; i < m_BufferLayouts.size(); i++) {
            m_WGPUVertexBufferLayouts[i] = VertexLayout::Get(m_BufferLayouts[i]);
        }
        return *this;
    }

    VertexState &VertexState::addVertexLayout(const VertexLayout &layout) {
        m_BufferLayouts.emplace_back(layout);
        m_WGPUVertexBufferLayouts.emplace_back(VertexLayout::Get(m_BufferLayouts.back()));
        return *this;
    }

    WGPUVertexState VertexState::Get(const VertexState &state) {
        return WGPUVertexState{
            .module = state.m_Shader.get(),
            .entryPoint = state.m_EntryPoint.c_str(),
            .bufferCount = state.m_WGPUVertexBufferLayouts.size(),
            .buffers = state.m_WGPUVertexBufferLayouts.data(),
        };
    }

    PrimitiveState::PrimitiveState() {
        m_PrimitiveState = WGPUPrimitiveState{.topology = WGPUPrimitiveTopology_TriangleList};
    }

    PrimitiveState &PrimitiveState::setTopology(PrimitiveTopology topology) {
        m_PrimitiveState.topology = static_cast<WGPUPrimitiveTopology>(topology);
        return *this;
    }

    PrimitiveState &PrimitiveState::setStripIndexFormat(IndexFormat format) {
        m_PrimitiveState.stripIndexFormat = static_cast<WGPUIndexFormat>(format);
        return *this;
    }

    PrimitiveState &PrimitiveState::setFrontFace(FrontFace frontFace) {
        m_PrimitiveState.frontFace = static_cast<WGPUFrontFace>(frontFace);
        return *this;
    }

    PrimitiveState &PrimitiveState::setCullMode(CullMode cullMode) {
        m_PrimitiveState.cullMode = static_cast<WGPUCullMode>(cullMode);
        return *this;
    }

    WGPUPrimitiveState PrimitiveState::get() const {
        return m_PrimitiveState;
    }

    DepthStencilState::DepthStencilState(TextureFormat format) {
        m_DepthStencilState = WGPUDepthStencilState{
            .format = static_cast<WGPUTextureFormat>(format),
            .depthWriteEnabled = true,
            .depthCompare = WGPUCompareFunction_Less,
            .stencilFront = WGPUStencilFaceState{
                .compare = WGPUCompareFunction_Always,
                .failOp = WGPUStencilOperation_Keep,
                .depthFailOp = WGPUStencilOperation_Keep,
                .passOp = WGPUStencilOperation_Keep,
            },
            .stencilBack = WGPUStencilFaceState{
                .compare = WGPUCompareFunction_Always,
                .failOp = WGPUStencilOperation_Keep,
                .depthFailOp = WGPUStencilOperation_Keep,
                .passOp = WGPUStencilOperation_Keep,
            },
            .stencilReadMask = 0xFF,
            .stencilWriteMask = 0xFF,
            .depthBias = 0,
            .depthBiasSlopeScale = 0.0f,
            .depthBiasClamp = 0.0f,
        };
    }

    DepthStencilState &DepthStencilState::setDepthWrite(bool enabled) {
        m_DepthStencilState.depthWriteEnabled = enabled;
        return *this;
    }

    DepthStencilState &DepthStencilState::setDepthCompare(CompareFunction compareFunction) {
        m_DepthStencilState.depthCompare = static_cast<WGPUCompareFunction>(compareFunction);
        return *this;
    }

    DepthStencilState &DepthStencilState::setDepthBias(std::int32_t bias) {
        m_DepthStencilState.depthBias = bias;
        return *this;
    }

    DepthStencilState &DepthStencilState::setDepthBiasSlopeScale(float scale) {
        m_DepthStencilState.depthBiasSlopeScale = scale;
        return *this;
    }

    DepthStencilState &DepthStencilState::setDepthBiasClamp(float clamp) {
        m_DepthStencilState.depthBiasClamp = clamp;
        return *this;
    }

    DepthStencilState &DepthStencilState::setStencilFront(StencilFaceState stencilFaceState) {
        m_DepthStencilState.stencilFront = stencilFaceState.get();
        return *this;
    }

    DepthStencilState &DepthStencilState::setStencilBack(StencilFaceState stencilFaceState) {
        m_DepthStencilState.stencilBack = stencilFaceState.get();
        return *this;
    }

    DepthStencilState &DepthStencilState::setStencilReadMask(std::uint32_t mask) {
        m_DepthStencilState.stencilReadMask = mask;
        return *this;
    }

    DepthStencilState &DepthStencilState::setStencilWriteMask(std::uint32_t mask) {
        m_DepthStencilState.stencilWriteMask = mask;
        return *this;
    }

    WGPUDepthStencilState DepthStencilState::get() const {
        return m_DepthStencilState;
    }

    MultisampleState::MultisampleState() {
        m_MultisampleState = WGPUMultisampleState{
            .count = 1,
            .mask = 0xFFFFFFFF,
            .alphaToCoverageEnabled = false,
        };
    }

    MultisampleState &MultisampleState::setCount(std::uint32_t count) {
        m_MultisampleState.count = count;
        return *this;
    }

    MultisampleState &MultisampleState::setMask(std::uint32_t mask) {
        m_MultisampleState.mask = mask;
        return *this;
    }

    MultisampleState &MultisampleState::setAlphaToCoverage(bool enabled) {
        m_MultisampleState.alphaToCoverageEnabled = enabled;
        return *this;
    }

    WGPUMultisampleState MultisampleState::get() const {
        return m_MultisampleState;
    }

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

    RenderPipeline::RenderPipeline(const std::string &label, const WGPURenderPipeline &handle, const PipelineLayout &pipelineLayout)
        : m_Label{label}, m_RenderPipeline{handle}, m_PipelineLayout{pipelineLayout} {
        assert(handle != nullptr && "RenderPipeline handle cannot be nullptr");
    }

    RenderPipeline::RenderPipeline()
        : m_Label{}, m_RenderPipeline{nullptr}, m_PipelineLayout{} {}

    RenderPipeline::~RenderPipeline() {
        if (m_RenderPipeline)
            wgpuRenderPipelineRelease(m_RenderPipeline);
    }

    RenderPipeline::RenderPipeline(const RenderPipeline &other)
        : m_Label{other.m_Label}, m_RenderPipeline{other.m_RenderPipeline}, m_PipelineLayout{other.m_PipelineLayout} {
        assert(other.m_RenderPipeline != nullptr && "RenderPipeline handle cannot be nullptr");
        wgpuRenderPipelineReference(m_RenderPipeline);
    }

    RenderPipeline::RenderPipeline(RenderPipeline &&other)
        : m_Label{other.m_Label}, m_RenderPipeline{other.m_RenderPipeline}, m_PipelineLayout{std::move(other.m_PipelineLayout)} {
        assert(other.m_RenderPipeline != nullptr && "RenderPipeline handle cannot be nullptr");
        other.m_RenderPipeline = nullptr;
        other.m_Label = "";
    }

    RenderPipeline &RenderPipeline::operator=(const RenderPipeline &other) {
        assert(other.m_RenderPipeline != nullptr && "RenderPipeline handle cannot be nullptr");

        if (&other != this) {
            if (m_RenderPipeline) wgpuRenderPipelineRelease(m_RenderPipeline);

            m_RenderPipeline = other.m_RenderPipeline;
            m_Label = other.m_Label;
            m_PipelineLayout = other.m_PipelineLayout;

            wgpuRenderPipelineReference(m_RenderPipeline);
        }

        return *this;
    }

    RenderPipeline &RenderPipeline::operator=(RenderPipeline &&other) {
        assert(other.m_RenderPipeline != nullptr && "RenderPipeline handle cannot be nullptr");

        if (&other != this) {
            if (m_RenderPipeline) wgpuRenderPipelineRelease(m_RenderPipeline);

            m_RenderPipeline = other.m_RenderPipeline;
            m_Label = other.m_Label;
            m_PipelineLayout = std::move(other.m_PipelineLayout);

            other.m_RenderPipeline = nullptr;
            other.m_Label = "";
        }

        return *this;
    }

    std::string RenderPipeline::getLabel() const {
        assert(m_RenderPipeline != nullptr && "RenderPipeline handle cannot be nullptr");

        return m_Label;
    }

    WGPURenderPipeline RenderPipeline::get() const {
        assert(m_RenderPipeline != nullptr && "RenderPipeline handle cannot be nullptr");

        return m_RenderPipeline;
    }

    const PipelineLayout &RenderPipeline::getLayoutRef() const {
        assert(m_RenderPipeline != nullptr && "RenderPipeline handle cannot be nullptr");

        return m_PipelineLayout;
    }

    BindGroupLayout RenderPipeline::getBindGroupLayout(std::uint32_t groupIndex) const {
        assert(m_RenderPipeline != nullptr && "RenderPipeline handle cannot be nullptr");

        return BindGroupLayout{wgpuRenderPipelineGetBindGroupLayout(m_RenderPipeline, groupIndex)};
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

    RenderPipelineBuilder::RenderPipelineBuilder() {
        m_Label = "Yulduz Render Pipeline";
    }

    RenderPipelineBuilder &RenderPipelineBuilder::setLabel(const std::string &label) {
        m_Label = label;
        return *this;
    }

    RenderPipelineBuilder &RenderPipelineBuilder::setPrimtiveState(const PrimitiveState &state) {
        m_PrimitiveState = state;
        return *this;
    }

    RenderPipelineBuilder &RenderPipelineBuilder::setMultisampleState(const MultisampleState &state) {
        m_MultisampleState = state;
        return *this;
    }

    RenderPipelineBuilder &RenderPipelineBuilder::setVertexStateReq(const VertexState &state) {
        m_VertexState = state;
        return *this;
    }

    RenderPipelineBuilder &RenderPipelineBuilder::setFragmentStateReq(const FragmentState &state) {
        m_FragmentState = state;
        return *this;
    }

    RenderPipelineBuilder &RenderPipelineBuilder::setDepthStencilState(const DepthStencilState &state) {
        m_DepthStencilState = state;
        return *this;
    }

    RenderPipeline RenderPipelineBuilder::build(const PipelineLayout &layout, const GraphicsContext &context) {
        if (!m_VertexState) {
            YZFATAL("Render Pipeline: '{}' Vertex State is not set!", m_Label);
            throw std::runtime_error("Render Pipeline: '" + m_Label + "' Vertex State is not set!");
        }
        if (!m_FragmentState) {
            YZFATAL("Render Pipeline: '{}' Fragment State is not set!", m_Label);
            throw std::runtime_error("Render Pipeline: '" + m_Label + "' Fragment State is not set!");
        }

        WGPUVertexState vertexState = VertexState::Get(m_VertexState.value());
        WGPUFragmentState fragmentState = FragmentState::Get(m_FragmentState.value());

        WGPUDepthStencilState depthStencilState;
        if (m_DepthStencilState) {
            depthStencilState = m_DepthStencilState.value().get();
        }

        WGPURenderPipelineDescriptor descriptor{
            .label = m_Label.c_str(),
            .layout = layout.get(),
            .vertex = vertexState,
            .primitive = m_PrimitiveState.get(),
            .depthStencil = m_DepthStencilState ? &depthStencilState : nullptr,
            .multisample = m_MultisampleState.get(),
            .fragment = &fragmentState,
        };

        return RenderPipeline{m_Label, wgpuDeviceCreateRenderPipeline(context.getDevice(), &descriptor), layout};
    }

    RenderPipeline RenderPipelineBuilder::buildAutoLayout(const GraphicsContext &context) {
        if (!m_VertexState) {
            YZFATAL("Render Pipeline: '{}' Vertex State is not set!", m_Label);
            throw std::runtime_error("Render Pipeline: '" + m_Label + "' Vertex State is not set!");
        }
        if (!m_FragmentState) {
            YZFATAL("Render Pipeline: '{}' Fragment State is not set!", m_Label);
            throw std::runtime_error("Render Pipeline: '" + m_Label + "' Fragment State is not set!");
        }

        WGPUVertexState vertexState = VertexState::Get(m_VertexState.value());
        WGPUFragmentState fragmentState = FragmentState::Get(m_FragmentState.value());

        WGPUDepthStencilState depthStencilState;
        if (m_DepthStencilState) {
            depthStencilState = m_DepthStencilState.value().get();
        }

        WGPURenderPipelineDescriptor descriptor{
            .label = m_Label.c_str(),
            .vertex = vertexState,
            .primitive = m_PrimitiveState.get(),
            .depthStencil = m_DepthStencilState ? &depthStencilState : nullptr,
            .multisample = m_MultisampleState.get(),
            .fragment = &fragmentState,
        };

        return RenderPipeline{m_Label, wgpuDeviceCreateRenderPipeline(context.getDevice(), &descriptor), PipelineLayout::Auto()};
    }
}  // namespace Yulduz
