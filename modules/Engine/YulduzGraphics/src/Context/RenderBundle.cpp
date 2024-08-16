#include <YulduzGraphics/Context/RenderBundle.hpp>

namespace Yulduz {
    RenderBundle::RenderBundle(const std::string &label,
                               const WGPURenderBundle &renderBundle,
                               const std::vector<RenderPipeline> &renderPipelines,
                               const std::vector<Buffer> &buffers,
                               const std::vector<BindGroup> &bindGroups)
        : m_Label{label}, m_RenderBundle{renderBundle}, m_RenderPipelines{renderPipelines}, m_Buffers{buffers}, m_BindGroups{bindGroups} {
        assert(m_RenderBundle != nullptr && "RenderBundle handle cannot be nullptr");
    }

    RenderBundle::RenderBundle()
        : m_Label{}, m_RenderBundle{nullptr}, m_RenderPipelines{}, m_Buffers{}, m_BindGroups{} {}

    RenderBundle::~RenderBundle() {
        if (m_RenderBundle)
            wgpuRenderBundleRelease(m_RenderBundle);
    }

    RenderBundle::RenderBundle(const RenderBundle &other)
        : m_Label{other.m_Label}, m_RenderBundle{other.m_RenderBundle}, m_RenderPipelines{other.m_RenderPipelines}, m_Buffers{other.m_Buffers}, m_BindGroups{other.m_BindGroups} {
        assert(m_RenderBundle != nullptr && "RenderBundle handle cannot be nullptr");
        wgpuRenderBundleReference(m_RenderBundle);
    }

    RenderBundle::RenderBundle(RenderBundle &&other)
        : m_Label{other.m_Label}, m_RenderBundle{other.m_RenderBundle}, m_RenderPipelines{std::move(other.m_RenderPipelines)}, m_Buffers{std::move(other.m_Buffers)}, m_BindGroups{std::move(other.m_BindGroups)} {
        assert(m_RenderBundle != nullptr && "RenderBundle handle cannot be nullptr");
        other.m_RenderBundle = nullptr;
        other.m_Label = "";
        other.m_RenderPipelines.clear();
        other.m_Buffers.clear();
        other.m_BindGroups.clear();
    }

    RenderBundle &RenderBundle::operator=(const RenderBundle &other) {
        assert(other.m_RenderBundle != nullptr && "RenderBundle handle cannot be nullptr");

        if (&other != this) {
            if (m_RenderBundle) wgpuRenderBundleRelease(m_RenderBundle);

            m_RenderBundle = other.m_RenderBundle;
            m_Label = other.m_Label;
            m_RenderPipelines = other.m_RenderPipelines;
            m_Buffers = other.m_Buffers;
            m_BindGroups = other.m_BindGroups;

            wgpuRenderBundleReference(m_RenderBundle);
        }

        return *this;
    }

    RenderBundle &RenderBundle::operator=(RenderBundle &&other) {
        assert(other.m_RenderBundle != nullptr && "RenderBundle handle cannot be nullptr");

        if (&other != this) {
            if (m_RenderBundle) wgpuRenderBundleRelease(m_RenderBundle);

            m_RenderBundle = other.m_RenderBundle;
            m_Label = other.m_Label;
            m_RenderPipelines = std::move(other.m_RenderPipelines);
            m_Buffers = std::move(other.m_Buffers);
            m_BindGroups = std::move(other.m_BindGroups);

            other.m_RenderBundle = nullptr;
            other.m_Label = "";
            other.m_RenderPipelines.clear();
            other.m_Buffers.clear();
            other.m_BindGroups.clear();
        }

        return *this;
    }

    std::string RenderBundle::getLabel() const {
        assert(m_RenderBundle != nullptr && "RenderBundle handle cannot be nullptr");

        return m_Label;
    }

    WGPURenderBundle RenderBundle::get() const {
        assert(m_RenderBundle != nullptr && "RenderBundle handle cannot be nullptr");

        return m_RenderBundle;
    }

    RenderBundleEncoder::RenderBundleEncoder(const std::string &label, const WGPURenderBundleEncoder &handle)
        : m_Label{label}, m_RenderBundleEncoder{handle} {
        assert(m_RenderBundleEncoder != nullptr && "RenderBundleEncoder handle cannot be nullptr");
    }

    RenderBundleEncoder::RenderBundleEncoder()
        : m_Label{}, m_RenderBundleEncoder{nullptr} {}

    RenderBundleEncoder::~RenderBundleEncoder() {
        if (m_RenderBundleEncoder)
            wgpuRenderBundleEncoderRelease(m_RenderBundleEncoder);
    }

    RenderBundleEncoder::RenderBundleEncoder(const RenderBundleEncoder &other)
        : m_Label{other.m_Label}, m_RenderBundleEncoder{other.m_RenderBundleEncoder} {
        assert(other.m_RenderBundleEncoder != nullptr && "RenderBundleEncoder handle cannot be nullptr");
        wgpuRenderBundleEncoderReference(m_RenderBundleEncoder);
    }

    RenderBundleEncoder::RenderBundleEncoder(RenderBundleEncoder &&other)
        : m_Label{other.m_Label}, m_RenderBundleEncoder{other.m_RenderBundleEncoder} {
        assert(other.m_RenderBundleEncoder != nullptr && "RenderBundleEncoder handle cannot be nullptr");
        other.m_RenderBundleEncoder = nullptr;
        other.m_Label = "";
    }

    RenderBundleEncoder &RenderBundleEncoder::operator=(const RenderBundleEncoder &other) {
        assert(other.m_RenderBundleEncoder != nullptr && "RenderBundleEncoder handle cannot be nullptr");

        if (&other != this) {
            if (m_RenderBundleEncoder) wgpuRenderBundleEncoderRelease(m_RenderBundleEncoder);

            m_RenderBundleEncoder = other.m_RenderBundleEncoder;
            m_Label = other.m_Label;

            wgpuRenderBundleEncoderReference(m_RenderBundleEncoder);
        }

        return *this;
    }

    RenderBundleEncoder &RenderBundleEncoder::operator=(RenderBundleEncoder &&other) {
        assert(other.m_RenderBundleEncoder != nullptr && "RenderBundleEncoder handle cannot be nullptr");

        if (&other != this) {
            if (m_RenderBundleEncoder) wgpuRenderBundleEncoderRelease(m_RenderBundleEncoder);

            m_RenderBundleEncoder = other.m_RenderBundleEncoder;
            m_Label = other.m_Label;

            other.m_RenderBundleEncoder = nullptr;
            other.m_Label = "";
        }

        return *this;
    }

    void RenderBundleEncoder::setPipeline(const RenderPipeline &pipeline) {
        assert(m_RenderBundleEncoder != nullptr && "RenderBundleEncoder handle cannot be nullptr");

        wgpuRenderBundleEncoderSetPipeline(m_RenderBundleEncoder, pipeline.get());
        m_RenderPipelines.emplace_back(pipeline);
    }

    void RenderBundleEncoder::setBindGroups(const std::vector<BindGroup> &bindGroups) {
        assert(m_RenderBundleEncoder != nullptr && "RenderBundleEncoder handle cannot be nullptr");

        m_BindGroups.reserve(m_BindGroups.size() + bindGroups.size());

        for (std::uint32_t groupIndex = 0; groupIndex < bindGroups.size(); groupIndex++) {
            wgpuRenderBundleEncoderSetBindGroup(m_RenderBundleEncoder, groupIndex, bindGroups[groupIndex].get(), 0, nullptr);
            m_BindGroups.emplace_back(bindGroups[groupIndex]);
        }
    }

    void RenderBundleEncoder::setBindGroup(std::uint32_t groupIndex, const BindGroup &bindGroup) {
        assert(m_RenderBundleEncoder != nullptr && "RenderBundleEncoder handle cannot be nullptr");

        wgpuRenderBundleEncoderSetBindGroup(m_RenderBundleEncoder, groupIndex, bindGroup.get(), 0, nullptr);
        m_BindGroups.emplace_back(bindGroup);
    }

    void RenderBundleEncoder::setVertexBuffers(const std::vector<VertexBuffer> &vertexBuffers) {
        assert(m_RenderBundleEncoder != nullptr && "RenderBundleEncoder handle cannot be nullptr");

        m_Buffers.reserve(m_Buffers.size() + vertexBuffers.size());
        for (std::uint32_t slot = 0; slot < vertexBuffers.size(); slot++) {
            wgpuRenderBundleEncoderSetVertexBuffer(m_RenderBundleEncoder, slot, vertexBuffers[slot].get(), 0, vertexBuffers[slot].getSize());
            m_Buffers.emplace_back(vertexBuffers[slot]);
        }
    }

    void RenderBundleEncoder::setVertexBuffer(std::uint32_t slot, const VertexBuffer &vertexBuffer) {
        assert(m_RenderBundleEncoder != nullptr && "RenderBundleEncoder handle cannot be nullptr");

        wgpuRenderBundleEncoderSetVertexBuffer(m_RenderBundleEncoder, slot, vertexBuffer.get(), 0, vertexBuffer.getSize());
        m_Buffers.emplace_back(vertexBuffer);
    }

    void RenderBundleEncoder::draw(std::uint32_t slot, const VertexBuffer &vertexBuffer) {
        assert(m_RenderBundleEncoder != nullptr && "RenderBundleEncoder handle cannot be nullptr");

        wgpuRenderBundleEncoderSetVertexBuffer(m_RenderBundleEncoder, slot, vertexBuffer.get(), 0, vertexBuffer.getSize());
        wgpuRenderBundleEncoderDraw(m_RenderBundleEncoder, vertexBuffer.getCount(), 1, 0, 0);
        m_Buffers.emplace_back(vertexBuffer);
    }

    void RenderBundleEncoder::drawIndexed(const IndexBuffer &indexBuffer) {
        assert(m_RenderBundleEncoder != nullptr && "RenderBundleEncoder handle cannot be nullptr");

        wgpuRenderBundleEncoderSetIndexBuffer(m_RenderBundleEncoder, indexBuffer.get(), static_cast<WGPUIndexFormat>(indexBuffer.getIndexFormat()), 0, indexBuffer.getSize());
        wgpuRenderBundleEncoderDrawIndexed(m_RenderBundleEncoder, indexBuffer.getCount(), 1, 0, 0, 0);
        m_Buffers.emplace_back(indexBuffer);
    }

    RenderBundle RenderBundleEncoder::finish(const std::string &label) {
        assert(m_RenderBundleEncoder != nullptr && "RenderBundleEncoder handle cannot be nullptr");

        WGPURenderBundleDescriptor descriptor{.label = label.c_str()};

        return RenderBundle{label, wgpuRenderBundleEncoderFinish(m_RenderBundleEncoder, &descriptor), m_RenderPipelines, m_Buffers, m_BindGroups};
    }

    RenderBundleEncoderBuilder::RenderBundleEncoderBuilder() {
        m_Label = "Yulduz RenderBundleEncoder";
        m_DepthStencilFormat = TextureFormat::Undefined;
        m_SampleCount = 1;
        m_DepthReadOnly = false;
        m_StencilReadOnly = false;
    }

    RenderBundleEncoderBuilder &RenderBundleEncoderBuilder::setLabel(const std::string &label) {
        m_Label = label;
        return *this;
    }

    RenderBundleEncoderBuilder &RenderBundleEncoderBuilder::setColorFormats(const std::vector<TextureFormat> &colorFormats) {
        m_ColorFormats = colorFormats;
        return *this;
    }

    RenderBundleEncoderBuilder &RenderBundleEncoderBuilder::addColorFormat(TextureFormat colorFormat) {
        m_ColorFormats.emplace_back(colorFormat);
        return *this;
    }

    RenderBundleEncoderBuilder &RenderBundleEncoderBuilder::setDepthStencilFormat(TextureFormat depthStencilFormat) {
        m_DepthStencilFormat = depthStencilFormat;
        return *this;
    }

    RenderBundleEncoderBuilder &RenderBundleEncoderBuilder::setSampleCount(std::uint32_t sampleCount) {
        m_SampleCount = sampleCount;
        return *this;
    }

    RenderBundleEncoderBuilder &RenderBundleEncoderBuilder::setDepthReadOnly(bool depthReadOnly) {
        m_DepthReadOnly = depthReadOnly;
        return *this;
    }

    RenderBundleEncoderBuilder &RenderBundleEncoderBuilder::setStencilReadOnly(bool stencilReadOnly) {
        m_StencilReadOnly = stencilReadOnly;
        return *this;
    }

    RenderBundleEncoder RenderBundleEncoderBuilder::build(const GraphicsContext &context) {
        WGPURenderBundleEncoderDescriptor descriptor{
            .label = m_Label.c_str(),
            .colorFormatCount = static_cast<std::uint32_t>(m_ColorFormats.size()),
            .colorFormats = (WGPUTextureFormat *)m_ColorFormats.data(),
            .depthStencilFormat = static_cast<WGPUTextureFormat>(m_DepthStencilFormat),
            .sampleCount = m_SampleCount,
            .depthReadOnly = m_DepthReadOnly,
            .stencilReadOnly = m_StencilReadOnly,
        };

        return RenderBundleEncoder{m_Label, wgpuDeviceCreateRenderBundleEncoder(context.getDevice(), &descriptor)};
    }
}  // namespace Yulduz
