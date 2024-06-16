#pragma once

#include <Yulduz/Core.hpp>
#include <YulduzRender/Enums.hpp>

namespace Yulduz {
    class RenderContext;
    class RenderBundle;
    class Texture;
    class Buffer;

    class ImageCopyTexture {
       public:
        ImageCopyTexture(const std::shared_ptr<Texture> &texture);
        ~ImageCopyTexture() = default;

        ImageCopyTexture &setMipLevel(uint32_t mipLevel);
        ImageCopyTexture &setOrigin3D(uint32_t x, uint32_t y, uint32_t z);
        ImageCopyTexture &setAspect(TextureAspect aspect);

        WGPUImageCopyTexture get() const;
        const std::shared_ptr<Texture> &getTexture() const;

       private:
        std::shared_ptr<Texture> m_Texture;
        WGPUImageCopyTexture m_ImageCopyTexture;
    };
    
    class CommandBuffer {
       public:
        CommandBuffer(const std::string &label, const WGPUCommandBuffer &commandBuffer);
        ~CommandBuffer();

        std::string getLabel() const;
        WGPUCommandBuffer get() const;

       private:
        std::string m_Label;
        WGPUCommandBuffer m_CommandBuffer;
    };

    class CommandEncoder {
       public:
        CommandEncoder(const std::string &label, const WGPUCommandEncoder &commandEncoder);
        ~CommandEncoder();

        void copyTextureToTexture(const ImageCopyTexture &src, const ImageCopyTexture &dst);
        void copyBufferToBuffer(const std::shared_ptr<Buffer> &src, const std::shared_ptr<Buffer> &dst);
        void copyTextureToBuffer(const ImageCopyTexture &src, const std::shared_ptr<Buffer> &dst);
        void copyBufferToTexture(const std::shared_ptr<Buffer> &src, const ImageCopyTexture &dst);

        std::string getLabel() const;
        WGPUCommandEncoder get() const;

        std::shared_ptr<CommandBuffer> finish(const std::string &label = "Yulduz Command Buffer");

       private:
        std::string m_Label;
        WGPUCommandEncoder m_CommandEncoder;
    };

    class CommandEncoderBuilder {
       public:
        CommandEncoderBuilder();
        ~CommandEncoderBuilder() = default;

        inline static CommandEncoderBuilder New() { return CommandEncoderBuilder(); }

        CommandEncoderBuilder &setLabel(const std::string &label);

        std::shared_ptr<CommandEncoder> build(const std::shared_ptr<RenderContext> &context);

       private:
        std::string m_Label;
    };
}  // namespace Yulduz
