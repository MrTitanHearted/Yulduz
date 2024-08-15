#pragma once

#include <Yulduz/Core.hpp>
#include <YulduzGraphics/Enums.hpp>

namespace Yulduz {
    class CommandEncoder;
    class Texture;

    class ColorAttachment {
       public:
        ColorAttachment(const Texture &target);
        ~ColorAttachment() = default;

        inline static ColorAttachment New(const Texture &target) { return ColorAttachment(target); }

        ColorAttachment &setTarget(const Texture &target);
        ColorAttachment &setResolveTarget(const Texture &resolveTarget);
        ColorAttachment &setLoadOp(LoadOp loadOp);
        ColorAttachment &setStoreOp(StoreOp storeOp);
        ColorAttachment &setClearColor4(float r, float g, float b, float a);
        ColorAttachment &setClearColor3(float r, float g, float b);
        ColorAttachment &setClearColor4(const std::array<float, 4> &color);
        ColorAttachment &setClearColor3(const std::array<float, 3> &color);

        WGPURenderPassColorAttachment get() const;

       private:
        WGPURenderPassColorAttachment m_Attachment;
    };

    class DepthStencilAttachment {
       public:
        DepthStencilAttachment(const Texture &target);
        ~DepthStencilAttachment() = default;

        inline static DepthStencilAttachment New(const Texture &target) { return DepthStencilAttachment(target); }

        DepthStencilAttachment &setTarget(const Texture &target);
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
        RenderPass(const std::string &label, const WGPURenderPassEncoder &handle);

        RenderPass();
        ~RenderPass();

        RenderPass(const RenderPass &other);
        RenderPass(RenderPass &&other);

        RenderPass &operator=(const RenderPass &other);
        RenderPass &operator=(RenderPass &&other);

        void finish();

        WGPURenderPassEncoder get() const;
        std::string getLabel() const;

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
        RenderPassBuilder &addColorAttachment(const ColorAttachment &attachment);
        RenderPassBuilder &setColorAttachments(const std::vector<ColorAttachment> &attachments);
        RenderPassBuilder &setDepthStencilAttachment(const DepthStencilAttachment &attachment);

        RenderPass build(const CommandEncoder &encoder);

       private:
        std::string m_Label;
        std::vector<ColorAttachment> m_ColorAttachments;
        std::optional<DepthStencilAttachment> m_DepthStencilAttachment;
    };
}  // namespace Yulduz
