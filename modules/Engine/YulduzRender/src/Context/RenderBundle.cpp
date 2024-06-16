#include <YulduzRender/Context/RenderBundle.hpp>
#include <YulduzRender/Context.hpp>

namespace Yulduz {
    RenderBundle::RenderBundle(const std::string &label,
                               const WGPURenderBundle &renderBundle,
                               const std::vector<std::shared_ptr<RenderPipeline>> &pipelines,
                               const std::vector<std::shared_ptr<Buffer>> &buffers,
                               const std::vector<std::shared_ptr<BindGroup>> &bindGroups) {
        YZDEBUG("Initializing Render Bundle: '{}'", label);

        m_Label = label;
        m_RenderBundle = renderBundle;
        m_RenderPipelines = pipelines;
        m_Buffers = buffers;
        m_BindGroups = bindGroups;
    }

    RenderBundle::~RenderBundle() {
        YZDEBUG("Releasing Render Bundle: '{}'", m_Label);

        wgpuRenderBundleRelease(m_RenderBundle);
    }

    std::string RenderBundle::getLabel() const {
        return m_Label;
    }

    WGPURenderBundle RenderBundle::get() const {
        return m_RenderBundle;
    }

    RenderBundleEncoder::RenderBundleEncoder(const std::string &label, const WGPURenderBundleEncoder &encoder) {
        YZINFO("Initializing Render Bundle Encoder: '{}'", label);

        m_Label = label;
        m_RenderBundleEncoder = encoder;
    }

    RenderBundleEncoder::~RenderBundleEncoder() {
        YZINFO("Releasing Render Bundle Encoder: '{}'", m_Label);

        wgpuRenderBundleEncoderRelease(m_RenderBundleEncoder);
    }

    void RenderBundleEncoder::setRenderPipeline(const std::shared_ptr<RenderPipeline> &pipeline) {
        m_RenderPipelines.emplace_back(pipeline);
        wgpuRenderBundleEncoderSetPipeline(m_RenderBundleEncoder, pipeline->get());
    }

    void RenderBundleEncoder::setVertexBuffers(const std::vector<std::shared_ptr<VertexBuffer>> &buffers) {
        for (size_t i = 0; i < buffers.size(); i++) {
            const std::shared_ptr<VertexBuffer> &buffer = buffers[i];
            m_Buffers.emplace_back(buffer);
            wgpuRenderBundleEncoderSetVertexBuffer(m_RenderBundleEncoder, i, buffer->get(), 0, buffer->getSize());
        }
    }

    void RenderBundleEncoder::setVertexBuffer(uint32_t slot, const std::shared_ptr<VertexBuffer> &buffer) {
        m_Buffers.emplace_back(buffer);
        wgpuRenderBundleEncoderSetVertexBuffer(m_RenderBundleEncoder, slot, buffer->get(), 0, buffer->getSize());
    }

    void RenderBundleEncoder::setBindGroups(const std::vector<std::shared_ptr<BindGroup>> &bindGroups) {
        for (size_t i = 0; i < bindGroups.size(); i++) {
            const std::shared_ptr<BindGroup> &bindGroup = bindGroups[i];
            m_BindGroups.emplace_back(bindGroup);
            wgpuRenderBundleEncoderSetBindGroup(m_RenderBundleEncoder, i, bindGroup->get(), 0, nullptr);
        }
    }

    void RenderBundleEncoder::setBindGroup(uint32_t groupIndex, const std::shared_ptr<BindGroup> &bindGroup) {
        m_BindGroups.emplace_back(bindGroup);
        wgpuRenderBundleEncoderSetBindGroup(m_RenderBundleEncoder, groupIndex, bindGroup->get(), 0, nullptr);
    }

    void RenderBundleEncoder::draw(uint32_t slot, const std::shared_ptr<VertexBuffer> &buffer) {
        m_Buffers.emplace_back(buffer);
        wgpuRenderBundleEncoderSetVertexBuffer(m_RenderBundleEncoder, slot, buffer->get(), 0, buffer->getSize());
        wgpuRenderBundleEncoderDraw(m_RenderBundleEncoder, buffer->getCount(), 1, 0, 0);
    }

    void RenderBundleEncoder::drawIndexed(const std::shared_ptr<IndexBuffer> &buffer) {
        m_Buffers.emplace_back(buffer);
        wgpuRenderBundleEncoderSetIndexBuffer(m_RenderBundleEncoder, buffer->get(), static_cast<WGPUIndexFormat>(buffer->getIndexFormat()), 0, buffer->getSize());
        wgpuRenderBundleEncoderDrawIndexed(m_RenderBundleEncoder, buffer->getCount(), 1, 0, 0, 0);
    }

    std::string RenderBundleEncoder::getLabel() const {
        return m_Label;
    }

    WGPURenderBundleEncoder RenderBundleEncoder::get() const {
        return m_RenderBundleEncoder;
    }

    std::shared_ptr<RenderBundle> RenderBundleEncoder::finish(const std::string &label) {
        WGPURenderBundleDescriptor descriptor{.label = label.c_str()};

        WGPURenderBundle renderBundle = wgpuRenderBundleEncoderFinish(m_RenderBundleEncoder, &descriptor);

        return std::make_shared<RenderBundle>(label, renderBundle, m_RenderPipelines, m_Buffers, m_BindGroups);
    }

    RenderBundleEncoderBuilder::RenderBundleEncoderBuilder() {
        m_Label = "Yulduz Render Bundle Encoder";
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
        m_ColorFormats.resize(colorFormats.size());
        for (size_t i = 0; i < colorFormats.size(); i++) {
            m_ColorFormats[i] = static_cast<WGPUTextureFormat>(colorFormats[i]);
        }
        return *this;
    }

    RenderBundleEncoderBuilder &RenderBundleEncoderBuilder::addColorFormats(const std::vector<TextureFormat> &colorFormats) {
        for (const TextureFormat &format : colorFormats) {
            m_ColorFormats.emplace_back(static_cast<WGPUTextureFormat>(format));
        }
        return *this;
    }

    RenderBundleEncoderBuilder &RenderBundleEncoderBuilder::addColorFormat(TextureFormat colorFormat) {
        m_ColorFormats.emplace_back(static_cast<WGPUTextureFormat>(colorFormat));
        return *this;
    }

    RenderBundleEncoderBuilder &RenderBundleEncoderBuilder::setDepthStencilFormat(TextureFormat depthStencilFormat) {
        m_DepthStencilFormat = depthStencilFormat;
        return *this;
    }

    RenderBundleEncoderBuilder &RenderBundleEncoderBuilder::setSampleCount(uint32_t sampleCount) {
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

    std::shared_ptr<RenderBundleEncoder> RenderBundleEncoderBuilder::build(const std::shared_ptr<RenderContext> &context) {
        WGPURenderBundleEncoderDescriptor descriptor{
            .label = m_Label.c_str(),
            .colorFormatCount = m_ColorFormats.size(),
            .colorFormats = m_ColorFormats.data(),
            .depthStencilFormat = static_cast<WGPUTextureFormat>(m_DepthStencilFormat),
            .sampleCount = m_SampleCount,
            .depthReadOnly = m_DepthReadOnly,
            .stencilReadOnly = m_StencilReadOnly,
        };

        WGPURenderBundleEncoder encoder = wgpuDeviceCreateRenderBundleEncoder(context->getDevice(), &descriptor);

        return std::make_shared<RenderBundleEncoder>(m_Label, encoder);
    }

}  // namespace Yulduz
