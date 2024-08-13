#include <YulduzGraphics/Context/RenderPass.hpp>
#include <YulduzGraphics/Context/CommandEncoder.hpp>
#include <YulduzGraphics/Context/Texture.hpp>

namespace Yulduz {
    ColorAttachment::ColorAttachment(const Texture &target) {
        m_Attachment = WGPURenderPassColorAttachment{
            .view = target.getDefaultView(),
            .resolveTarget = nullptr,
            .loadOp = WGPULoadOp_Clear,
            .storeOp = WGPUStoreOp_Store,
            .clearValue = WGPUColor{.r = 0.0, .g = 0.0, .b = 0.0, .a = 0.0},
        };
    }

    ColorAttachment &ColorAttachment::setTarget(const Texture &target) {
        m_Attachment.view = target.getDefaultView();
        return *this;
    }

    ColorAttachment &ColorAttachment::setResolveTarget(const Texture &resolveTarget) {
        m_Attachment.resolveTarget = resolveTarget.getDefaultView();
        return *this;
    }

    ColorAttachment &ColorAttachment::setLoadOp(LoadOp loadOp) {
        m_Attachment.loadOp = static_cast<WGPULoadOp>(loadOp);
        return *this;
    }

    ColorAttachment &ColorAttachment::setStoreOp(StoreOp storeOp) {
        m_Attachment.storeOp = static_cast<WGPUStoreOp>(storeOp);
        return *this;
    }

    ColorAttachment &ColorAttachment::setClearColor4(float r, float g, float b, float a) {
        m_Attachment.clearValue = WGPUColor{.r = r, .g = g, .b = b, .a = a};
        return *this;
    }

    ColorAttachment &ColorAttachment::setClearColor3(float r, float g, float b) {
        m_Attachment.clearValue = WGPUColor{.r = r, .g = g, .b = b, .a = 1.0f};
        return *this;
    }

    ColorAttachment &ColorAttachment::setClearColor4(const std::array<float, 4> &color) {
        m_Attachment.clearValue = WGPUColor{.r = color[0], .g = color[1], .b = color[2], .a = color[3]};
        return *this;
    }

    ColorAttachment &ColorAttachment::setClearColor3(const std::array<float, 3> &color) {
        m_Attachment.clearValue = WGPUColor{.r = color[0], .g = color[1], .b = color[2], .a = 1.0f};
        return *this;
    }

    WGPURenderPassColorAttachment ColorAttachment::get() const {
        return m_Attachment;
    }

    DepthStencilAttachment::DepthStencilAttachment(const Texture &target) {
        m_Attachment = WGPURenderPassDepthStencilAttachment{
            .view = target.getDefaultView(),
            .depthLoadOp = WGPULoadOp_Clear,
            .depthStoreOp = WGPUStoreOp_Store,
            .depthClearValue = 1.0f,
            .stencilLoadOp = WGPULoadOp_Undefined,
            .stencilStoreOp = WGPUStoreOp_Undefined,
            .stencilClearValue = 0,
        };
    }

    DepthStencilAttachment &DepthStencilAttachment::setTarget(const Texture &target) {
        m_Attachment.view = target.getDefaultView();
        return *this;
    }

    DepthStencilAttachment &DepthStencilAttachment::setDepthLoadOp(LoadOp loadOp) {
        m_Attachment.depthLoadOp = static_cast<WGPULoadOp>(loadOp);
        return *this;
    }

    DepthStencilAttachment &DepthStencilAttachment::setDepthStoreOp(StoreOp storeOp) {
        m_Attachment.depthStoreOp = static_cast<WGPUStoreOp>(storeOp);
        return *this;
    }

    DepthStencilAttachment &DepthStencilAttachment::setDepthClearValue(float value) {
        m_Attachment.depthClearValue = value;
        return *this;
    }

    DepthStencilAttachment &DepthStencilAttachment::setDepthReadOnly(bool value) {
        m_Attachment.depthReadOnly = value;
        return *this;
    }

    DepthStencilAttachment &DepthStencilAttachment::setStencilLoadOp(LoadOp loadOp) {
        m_Attachment.stencilLoadOp = static_cast<WGPULoadOp>(loadOp);
        return *this;
    }

    DepthStencilAttachment &DepthStencilAttachment::setStencilStoreOp(StoreOp storeOp) {
        m_Attachment.stencilStoreOp = static_cast<WGPUStoreOp>(storeOp);
        return *this;
    }

    DepthStencilAttachment &DepthStencilAttachment::setStencilClearValue(std::uint32_t value) {
        m_Attachment.stencilClearValue = value;
        return *this;
    }

    WGPURenderPassDepthStencilAttachment DepthStencilAttachment::get() const {
        return m_Attachment;
    }

    RenderPass::RenderPass(const std::string &label, const WGPURenderPassEncoder &handle) {
        assert(handle != nullptr && "RenderPass handle cannot be nullptr");

        m_RenderPassEncoder = handle;
        m_Label = label;
    }

    RenderPass::~RenderPass() {
        if (m_RenderPassEncoder)
            wgpuRenderPassEncoderRelease(m_RenderPassEncoder);
    }

    RenderPass::RenderPass(const RenderPass &other) {
        assert(other.m_RenderPassEncoder != nullptr && "RenderPass handle cannot be nullptr");

        if (&other != this) {
            wgpuRenderPassEncoderRelease(m_RenderPassEncoder);

            m_RenderPassEncoder = other.m_RenderPassEncoder;
            m_Label = other.m_Label;

            wgpuRenderPassEncoderReference(m_RenderPassEncoder);
        }
    }

    RenderPass &RenderPass::operator=(const RenderPass &other) {
        assert(other.m_RenderPassEncoder != nullptr && "RenderPass handle cannot be nullptr");

        if (&other != this) {
            wgpuRenderPassEncoderRelease(m_RenderPassEncoder);

            m_RenderPassEncoder = other.m_RenderPassEncoder;
            m_Label = other.m_Label;

            wgpuRenderPassEncoderReference(m_RenderPassEncoder);
        }

        return *this;
    }

    RenderPass::RenderPass(RenderPass &&other) {
        assert(other.m_RenderPassEncoder != nullptr && "RenderPass handle cannot be nullptr");

        if (&other != this) {
            wgpuRenderPassEncoderRelease(m_RenderPassEncoder);

            m_RenderPassEncoder = other.m_RenderPassEncoder;
            m_Label = other.m_Label;

            other.m_RenderPassEncoder = nullptr;
            other.m_Label = "";
        }
    }

    RenderPass &RenderPass::operator=(RenderPass &&other) {
        assert(other.m_RenderPassEncoder != nullptr && "RenderPass handle cannot be nullptr");

        if (&other != this) {
            wgpuRenderPassEncoderRelease(m_RenderPassEncoder);

            m_RenderPassEncoder = other.m_RenderPassEncoder;
            m_Label = other.m_Label;

            other.m_RenderPassEncoder = nullptr;
            other.m_Label = "";
        }

        return *this;
    }

    void RenderPass::finish() {
        assert(m_RenderPassEncoder != nullptr && "RenderPass handle cannot be nullptr");

        wgpuRenderPassEncoderEnd(m_RenderPassEncoder);
    }

    WGPURenderPassEncoder RenderPass::get() const {
        assert(m_RenderPassEncoder != nullptr && "RenderPass handle cannot be nullptr");

        return m_RenderPassEncoder;
    }

    std::string RenderPass::getLabel() const {
        assert(m_RenderPassEncoder != nullptr && "RenderPass handle cannot be nullptr");
        
        return m_Label;
    }

    RenderPassBuilder::RenderPassBuilder() {
        m_Label = "Yulduz Render Pass";
    }

    RenderPassBuilder &RenderPassBuilder::setLabel(const std::string &label) {
        m_Label = label;
        return *this;
    }

    RenderPassBuilder &RenderPassBuilder::addColorAttachment(const ColorAttachment &attachment) {
        m_ColorAttachments.emplace_back(attachment);
        return *this;
    }

    RenderPassBuilder &RenderPassBuilder::setColorAttachments(const std::vector<ColorAttachment> &attachments) {
        m_ColorAttachments = attachments;
        return *this;
    }

    RenderPassBuilder &RenderPassBuilder::setDepthStencilAttachment(const DepthStencilAttachment &attachment) {
        m_DepthStencilAttachment = attachment;
        return *this;
    }

    RenderPass RenderPassBuilder::build(const CommandEncoder &encoder) {
        WGPURenderPassDescriptor descriptor{
            .label = m_Label.c_str(),
            .colorAttachmentCount = m_ColorAttachments.size(),
            .colorAttachments = (WGPURenderPassColorAttachment *)m_ColorAttachments.data(),
            .depthStencilAttachment = m_DepthStencilAttachment ? (WGPURenderPassDepthStencilAttachment *)&m_DepthStencilAttachment.value() : nullptr,
        };
        return RenderPass{m_Label, wgpuCommandEncoderBeginRenderPass(encoder.get(), &descriptor)};
    }
}  // namespace Yulduz