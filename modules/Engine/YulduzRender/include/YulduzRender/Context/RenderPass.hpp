#pragma once

#include <Yulduz/Core.hpp>
#include <YulduzRender/Enums.hpp>

namespace Yulduz {
    class CommandEncoder;
    class RenderBundle;
    class Texture;
    class RenderPipeline;
    class VertexBuffer;
    class IndexBuffer;
    class BindGroup;

    class ColorAttachment {
       public:
        ColorAttachment(const std::shared_ptr<Texture> &target);
        ~ColorAttachment() = default;

        inline static ColorAttachment New(const std::shared_ptr<Texture> &target) { return ColorAttachment(target); }

        ColorAttachment &setTarget(const std::shared_ptr<Texture> &target);
        ColorAttachment &setResolveTarget(const std::shared_ptr<Texture> &resolveTarget);
        ColorAttachment &setLoadOp(LoadOp loadOp);
        ColorAttachment &setStoreOp(StoreOp storeOp);
        ColorAttachment &setClearColor(float r, float g, float b, float a);

        WGPURenderPassColorAttachment get() const;

       private:
        WGPURenderPassColorAttachment m_Attachment;
    };

    class DepthStencilAttachment {
       public:
        DepthStencilAttachment(const std::shared_ptr<Texture> &target);
        ~DepthStencilAttachment() = default;

        inline static DepthStencilAttachment New(const std::shared_ptr<Texture> &target) { return DepthStencilAttachment(target); }

        DepthStencilAttachment &setTarget(const std::shared_ptr<Texture> &target);
        DepthStencilAttachment &setDepthLoadOp(LoadOp loadOp);
        DepthStencilAttachment &setDepthStoreOp(StoreOp storeOp);
        DepthStencilAttachment &setDepthClearValue(float value);
        DepthStencilAttachment &setDepthReadOnly(bool value);
        DepthStencilAttachment &setStencilLoadOp(LoadOp loadOp);
        DepthStencilAttachment &setStencilStoreOp(StoreOp storeOp);
        DepthStencilAttachment &setStencilClearValue(std::uint32_t value);

        WGPURenderPassDepthStencilAttachment get() const;

       private:
        WGPURenderPassDepthStencilAttachment m_Attachment;
    };

    class RenderPass {
       public:
        RenderPass(const std::string &label, const WGPURenderPassEncoder &encoder);
        ~RenderPass();

        void setRenderPipeline(const std::shared_ptr<RenderPipeline> &pipeline);
        void setVertexBuffers(const std::vector<std::shared_ptr<VertexBuffer>> &buffers);
        void setVertexBuffer(std::uint32_t slot, const std::shared_ptr<VertexBuffer> &buffer);
        void setBindGroups(const std::vector<std::shared_ptr<BindGroup>> &bindGroups);
        void setBindGroup(std::uint32_t groupIndex, const std::shared_ptr<BindGroup> &bindGroup);

        void draw(std::uint32_t slot, const std::shared_ptr<VertexBuffer> &buffer);
        void drawIndexed(const std::shared_ptr<IndexBuffer> &buffer);

        void executeRenderBundles(const std::vector<std::shared_ptr<RenderBundle>> &renderBundles);
        void executeRenderBundle(const std::shared_ptr<RenderBundle> &renderBundle);
        void finish();

        std::string getLabel() const;
        WGPURenderPassEncoder get() const;

       private:
        std::string m_Label;
        WGPURenderPassEncoder m_RenderPassEncoder;
    };

    class RenderPassBuilder {
       public:
        RenderPassBuilder();
        ~RenderPassBuilder() = default;

        inline static RenderPassBuilder New() { return RenderPassBuilder(); }

        RenderPassBuilder &setLabel(const std::string &label);
        RenderPassBuilder &addColorAttachment(ColorAttachment attachment);
        RenderPassBuilder &setDepthStencilAttachment(DepthStencilAttachment attachment);

        std::shared_ptr<RenderPass> build(const std::shared_ptr<CommandEncoder> &encoder);

       private:
        std::string m_Label;
        std::vector<WGPURenderPassColorAttachment> m_ColorAttachments;
        std::optional<WGPURenderPassDepthStencilAttachment> m_DepthStencilAttachments;
    };
}  // namespace Yulduz
