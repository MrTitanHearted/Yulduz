#include <YulduzRender/Context/Pipelines.hpp>
#include <YulduzRender/Context.hpp>

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

    ColorTargetState &ColorTargetState::setBlendState(BlendState blendState) {
        m_BlendState = blendState.get();
        return *this;
    }

    ColorTargetState &ColorTargetState::setColorWriteMask(ColorWriteMask writeMask) {
        m_ColorWriteMask = writeMask;
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

    VertexState::VertexState(const std::shared_ptr<Shader> &shader, const std::string &entryPoint) {
        m_EntryPoint = entryPoint;
        m_Shader = shader;
    }

    VertexState &VertexState::setVertexLayouts(const std::vector<VertexLayout> &layouts) {
        m_BufferLayouts = layouts;
        m_WGPUVertexBufferLayouts.resize(m_BufferLayouts.size());
        for (size_t i = 0; i < m_BufferLayouts.size(); i++) {
            m_WGPUVertexBufferLayouts[i] = VertexLayout::Get(m_BufferLayouts[i]);
        }
        return *this;
    }

    VertexState &VertexState::addVertexLayouts(const std::vector<VertexLayout> &layouts) {
        for (const VertexLayout &layout : layouts) {
            m_BufferLayouts.emplace_back(layout);
            m_WGPUVertexBufferLayouts.emplace_back(VertexLayout::Get(m_BufferLayouts[m_BufferLayouts.size() - 1]));
        }
        return *this;
    }

    VertexState &VertexState::addVertexLayout(const VertexLayout &layout) {
        m_BufferLayouts.emplace_back(layout);
        m_WGPUVertexBufferLayouts.emplace_back(VertexLayout::Get(m_BufferLayouts[m_BufferLayouts.size() - 1]));
        return *this;
    }

    WGPUVertexState VertexState::Get(const VertexState &state) {
        return WGPUVertexState{
            .module = state.m_Shader->get(),
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

    DepthStencilState::DepthStencilState() {
        m_DepthStencilState = WGPUDepthStencilState{
            .format = WGPUTextureFormat_Depth32Float,
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

    DepthStencilState &DepthStencilState::setFormat(TextureFormat format) {
        m_DepthStencilState.format = static_cast<WGPUTextureFormat>(format);
        return *this;
    }

    DepthStencilState &DepthStencilState::setDepthWrite(bool enabled) {
        m_DepthStencilState.depthWriteEnabled = enabled;
        return *this;
    }

    DepthStencilState &DepthStencilState::setDepthCompare(CompareFunction compareFunction) {
        m_DepthStencilState.depthCompare = static_cast<WGPUCompareFunction>(compareFunction);
        return *this;
    }

    DepthStencilState &DepthStencilState::setDepthBias(int32_t bias) {
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

    DepthStencilState &DepthStencilState::setStencilReadMask(uint32_t mask) {
        m_DepthStencilState.stencilReadMask = mask;
        return *this;
    }

    DepthStencilState &DepthStencilState::setStencilWriteMask(uint32_t mask) {
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

    MultisampleState &MultisampleState::setCount(uint32_t count) {
        m_MultisampleState.count = count;
        return *this;
    }

    MultisampleState &MultisampleState::setMask(uint32_t mask) {
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

    FragmentState::FragmentState(const std::shared_ptr<Shader> &shader, const std::string &entryPoint) {
        m_EntryPoint = entryPoint;
        m_Shader = shader;
    }

    FragmentState &FragmentState::setColorTargetStates(const std::vector<ColorTargetState> &states) {
        m_ColorTargetStates = states;
        m_WGPUColorTargetStates.resize(m_ColorTargetStates.size());
        for (size_t i = 0; i < m_ColorTargetStates.size(); i++) {
            m_WGPUColorTargetStates[i] = ColorTargetState::Get(m_ColorTargetStates[i]);
        }
        return *this;
    }

    FragmentState &FragmentState::addColorTargetStates(const std::vector<ColorTargetState> &states) {
        for (const ColorTargetState &state : states) {
            m_ColorTargetStates.emplace_back(state);
            m_WGPUColorTargetStates.emplace_back(ColorTargetState::Get(m_ColorTargetStates[m_ColorTargetStates.size() - 1]));
        }
        return *this;
    }

    FragmentState &FragmentState::addColorTargetState(const ColorTargetState &state) {
        m_ColorTargetStates.emplace_back(state);
        m_WGPUColorTargetStates.emplace_back(ColorTargetState::Get(m_ColorTargetStates[m_ColorTargetStates.size() - 1]));
        return *this;
    }

    WGPUFragmentState FragmentState::Get(const FragmentState &state) {
        return WGPUFragmentState{
            .module = state.m_Shader->get(),
            .entryPoint = state.m_EntryPoint.c_str(),
            .targetCount = state.m_WGPUColorTargetStates.size(),
            .targets = state.m_WGPUColorTargetStates.data(),
        };
    }

    PipelineLayout::PipelineLayout(const std::string &label, const WGPUPipelineLayout &layout) {
        YZDEBUG("Initializing Pipeline Layout: '{}'", label);

        m_Label = label;
        m_PipelineLayout = layout;
    }

    PipelineLayout::~PipelineLayout() {
        YZDEBUG("Releasing Pipeline Layout: '{}'", m_Label);

        wgpuPipelineLayoutRelease(m_PipelineLayout);
    }

    std::string PipelineLayout::getLabel() const {
        return m_Label;
    }

    WGPUPipelineLayout PipelineLayout::get() const {
        return m_PipelineLayout;
    }

    RenderPipeline::RenderPipeline(const std::string &label, const WGPURenderPipeline &pipeline, const std::shared_ptr<PipelineLayout> &layout) {
        YZDEBUG("Initializing Render Pipeline: '{}'", label);

        m_Label = label;
        m_RenderPipeline = pipeline;
        m_PipelineLayout = layout;
    }

    RenderPipeline::~RenderPipeline() {
        YZDEBUG("Releasing Render Pipeline: '{}'", m_Label);

        wgpuRenderPipelineRelease(m_RenderPipeline);
    }

    std::string RenderPipeline::getLabel() const {
        return m_Label;
    }

    WGPURenderPipeline RenderPipeline::get() const {
        return m_RenderPipeline;
    }

    std::shared_ptr<PipelineLayout> RenderPipeline::getLayout() const {
        return m_PipelineLayout;
    }

    PipelineLayoutBuilder::PipelineLayoutBuilder() {
        m_Label = "Yulduz Pipeline Layout";
    }

    PipelineLayoutBuilder &PipelineLayoutBuilder::setLabel(const std::string &label) {
        m_Label = label;
        return *this;
    }

    PipelineLayoutBuilder &PipelineLayoutBuilder::setBindGroupLayouts(const std::vector<std::shared_ptr<BindGroupLayout>> &layouts) {
        m_BindGroupLayouts = layouts;
        return *this;
    }

    PipelineLayoutBuilder &PipelineLayoutBuilder::addBindGroupLayouts(const std::vector<std::shared_ptr<BindGroupLayout>> &layouts) {
        for (const std::shared_ptr<BindGroupLayout> &layout : layouts) {
            m_BindGroupLayouts.emplace_back(layout);
        }
        return *this;
    }

    PipelineLayoutBuilder &PipelineLayoutBuilder::addBindGroupLayout(const std::shared_ptr<BindGroupLayout> &layout) {
        m_BindGroupLayouts.emplace_back(layout);
        return *this;
    }

    std::shared_ptr<PipelineLayout> PipelineLayoutBuilder::build(const std::shared_ptr<RenderContext> &context) {
        std::vector<WGPUBindGroupLayout> layouts;
        layouts.reserve(m_BindGroupLayouts.size());
        for (const std::shared_ptr<BindGroupLayout> &layout : m_BindGroupLayouts) {
            layouts.emplace_back(layout->get());
        }

        WGPUPipelineLayoutDescriptor descriptor{
            .label = m_Label.c_str(),
            .bindGroupLayoutCount = layouts.size(),
            .bindGroupLayouts = layouts.data(),
        };

        WGPUPipelineLayout layout = wgpuDeviceCreatePipelineLayout(context->getDevice(), &descriptor);

        return std::make_shared<PipelineLayout>(m_Label, layout);
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

    RenderPipelineBuilder &RenderPipelineBuilder::setVertexState(const VertexState &state) {
        m_VertexState = state;
        return *this;
    }

    RenderPipelineBuilder &RenderPipelineBuilder::setFragmentState(const FragmentState &state) {
        m_FragmentState = state;
        return *this;
    }

    RenderPipelineBuilder &RenderPipelineBuilder::setDepthStencilState(const DepthStencilState &state) {
        m_DepthStencilState = state;
        return *this;
    }

    std::shared_ptr<RenderPipeline> RenderPipelineBuilder::build(const std::shared_ptr<PipelineLayout> &layout, const std::shared_ptr<RenderContext> &context) {
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
            .layout = layout->get(),
            .vertex = vertexState,
            .primitive = m_PrimitiveState.get(),
            .depthStencil = m_DepthStencilState.has_value() ? &depthStencilState : nullptr,
            .multisample = m_MultisampleState.get(),
            .fragment = &fragmentState,
        };

        WGPURenderPipeline pipeline = wgpuDeviceCreateRenderPipeline(context->getDevice(), &descriptor);

        return std::make_shared<RenderPipeline>(m_Label, pipeline, layout);
    }
}  // namespace Yulduz
