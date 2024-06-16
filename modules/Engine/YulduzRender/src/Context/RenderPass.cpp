#include <YulduzRender/Context/RenderPass.hpp>
#include <YulduzRender/Context.hpp>

namespace Yulduz {
    ColorAttachment::ColorAttachment(const std::shared_ptr<Texture> &target) {
        m_Attachment = WGPURenderPassColorAttachment{
            .view = target->getView(),
            .resolveTarget = nullptr,
            .loadOp = WGPULoadOp_Clear,
            .storeOp = WGPUStoreOp_Store,
            .clearValue = WGPUColor{.r = 0.0, .g = 0.0, .b = 0.0, .a = 0.0},
        };
    }

    ColorAttachment &ColorAttachment::withTarget(const std::shared_ptr<Texture> &target) {
        m_Attachment.view = target->getView();
        return *this;
    }

    ColorAttachment &ColorAttachment::withResolveTarget(const std::shared_ptr<Texture> &resolveTarget) {
        m_Attachment.resolveTarget = resolveTarget->getView();
        return *this;
    }

    ColorAttachment &ColorAttachment::withLoadOp(LoadOp loadOp) {
        m_Attachment.loadOp = static_cast<WGPULoadOp>(loadOp);
        return *this;
    }

    ColorAttachment &ColorAttachment::withStoreOp(StoreOp storeOp) {
        m_Attachment.storeOp = static_cast<WGPUStoreOp>(storeOp);
        return *this;
    }

    ColorAttachment &ColorAttachment::withClearColor(float r, float g, float b, float a) {
        m_Attachment.clearValue = WGPUColor{.r = r, .g = g, .b = b, .a = a};
        return *this;
    }

    WGPURenderPassColorAttachment ColorAttachment::get() const {
        return m_Attachment;
    }

    DepthStencilAttachment::DepthStencilAttachment(const std::shared_ptr<Texture> &target) {
        m_Attachment = WGPURenderPassDepthStencilAttachment{
            .view = target->getView(),
            .depthLoadOp = WGPULoadOp_Clear,
            .depthStoreOp = WGPUStoreOp_Store,
            .depthClearValue = 1.0f,
            .stencilLoadOp = WGPULoadOp_Undefined,
            .stencilStoreOp = WGPUStoreOp_Undefined,
            .stencilClearValue = 0,
        };
    }

    DepthStencilAttachment &DepthStencilAttachment::withTarget(const std::shared_ptr<Texture> &target) {
        m_Attachment.view = target->getView();
        return *this;
    }

    DepthStencilAttachment &DepthStencilAttachment::withDepthLoadOp(LoadOp loadOp) {
        m_Attachment.depthLoadOp = static_cast<WGPULoadOp>(loadOp);
        return *this;
    }

    DepthStencilAttachment &DepthStencilAttachment::withDepthStoreOp(StoreOp storeOp) {
        m_Attachment.depthStoreOp = static_cast<WGPUStoreOp>(storeOp);
        return *this;
    }

    DepthStencilAttachment &DepthStencilAttachment::withDepthClearValue(float value) {
        m_Attachment.depthClearValue = value;
        return *this;
    }

    WGPURenderPassDepthStencilAttachment DepthStencilAttachment::get() const {
        return m_Attachment;
    }

    RenderPass::RenderPass(const std::string &label, const WGPURenderPassEncoder &encoder) {
        YZINFO("Initializing Render Pass: '{}'", label);

        m_Label = label;
        m_RenderPassEncoder = encoder;
    }

    RenderPass::~RenderPass() {
        YZINFO("Releasing Render Pass: '{}'", m_Label);
        wgpuRenderPassEncoderRelease(m_RenderPassEncoder);
    }

    void RenderPass::setRenderPipeline(const std::shared_ptr<RenderPipeline> &pipeline) {
        wgpuRenderPassEncoderSetPipeline(m_RenderPassEncoder, pipeline->get());
    }

    void RenderPass::setVertexBuffers(const std::vector<std::shared_ptr<VertexBuffer>> &buffers) {
        for (size_t i = 0; i < buffers.size(); i++) {
            wgpuRenderPassEncoderSetVertexBuffer(m_RenderPassEncoder, i, buffers[i]->get(), 0, buffers[i]->getSize());
        }
    }

    void RenderPass::setVertexBuffer(uint32_t slot, const std::shared_ptr<VertexBuffer> &buffer) {
        wgpuRenderPassEncoderSetVertexBuffer(m_RenderPassEncoder, slot, buffer->get(), 0, buffer->getSize());
    }

    void RenderPass::setBindGroups(const std::vector<std::shared_ptr<BindGroup>> &bindGroups) {
        for (size_t i = 0; i < bindGroups.size(); i++) {
            wgpuRenderPassEncoderSetBindGroup(m_RenderPassEncoder, i, bindGroups[i]->get(), 0, nullptr);
        }
    }

    void RenderPass::setBindGroup(uint32_t groupIndex, const std::shared_ptr<BindGroup> &bindGroup) {
        wgpuRenderPassEncoderSetBindGroup(m_RenderPassEncoder, groupIndex, bindGroup->get(), 0, nullptr);
    }

    void RenderPass::draw(uint32_t slot, const std::shared_ptr<VertexBuffer> &buffer) {
        wgpuRenderPassEncoderSetVertexBuffer(m_RenderPassEncoder, slot, buffer->get(), 0, buffer->getSize());
        wgpuRenderPassEncoderDraw(m_RenderPassEncoder, buffer->getCount(), 1, 0, 0);
    }

    void RenderPass::drawIndexed(const std::shared_ptr<IndexBuffer> &buffer) {
        wgpuRenderPassEncoderSetIndexBuffer(m_RenderPassEncoder, buffer->get(), static_cast<WGPUIndexFormat>(buffer->getIndexFormat()), 0, buffer->getSize());
        wgpuRenderPassEncoderDrawIndexed(m_RenderPassEncoder, buffer->getCount(), 1, 0, 0, 0);
    }

    void RenderPass::executeRenderBundles(const std::vector<std::shared_ptr<RenderBundle>> &renderBundles) {
        std::vector<WGPURenderBundle> bundles;
        bundles.reserve(renderBundles.size());
        for (const std::shared_ptr<RenderBundle> &renderBundle : renderBundles) {
            bundles.emplace_back(renderBundle->get());
        }
        wgpuRenderPassEncoderExecuteBundles(m_RenderPassEncoder, bundles.size(), bundles.data());
    }

    void RenderPass::executeRenderBundle(const std::shared_ptr<RenderBundle> &renderBundle) {
        WGPURenderBundle bundle = renderBundle->get();
        wgpuRenderPassEncoderExecuteBundles(m_RenderPassEncoder, 1, &bundle);
    }

    void RenderPass::finish() {
        wgpuRenderPassEncoderEnd(m_RenderPassEncoder);
    }

    std::string RenderPass::getLabel() const {
        return m_Label;
    }

    WGPURenderPassEncoder RenderPass::get() const {
        return m_RenderPassEncoder;
    }

    RenderPassBuilder::RenderPassBuilder() {
        m_Label = "Yulduz Render Pass";
    }

    RenderPassBuilder &RenderPassBuilder::setLabel(const std::string &label) {
        m_Label = label;
        return *this;
    }

    RenderPassBuilder &RenderPassBuilder::addColorAttachment(ColorAttachment attachment) {
        m_ColorAttachments.emplace_back(attachment.get());
        return *this;
    }

    RenderPassBuilder &RenderPassBuilder::setDepthStencilAttachment(DepthStencilAttachment attachment) {
        m_DepthStencilAttachments = attachment.get();
        return *this;
    }

    std::shared_ptr<RenderPass> RenderPassBuilder::build(const std::shared_ptr<CommandEncoder> &encoder) {
        WGPURenderPassDescriptor descriptor{
            .label = m_Label.c_str(),
            .colorAttachmentCount = m_ColorAttachments.size(),
            .colorAttachments = m_ColorAttachments.data(),
            .depthStencilAttachment = m_DepthStencilAttachments.has_value()
                                          ? &m_DepthStencilAttachments.value()
                                          : nullptr,
        };
        WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(encoder->get(), &descriptor);
        return std::make_shared<RenderPass>(m_Label, renderPassEncoder);
    }

}  // namespace Yulduz
