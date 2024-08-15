#pragma once

#include <Yulduz/Core.hpp>
#include <YulduzGraphics/Enums.hpp>
#include <YulduzGraphics/Context/Texture.hpp>

namespace Yulduz {
    class GraphicsContext;
    class Buffer;

    class ImageCopyTexture {
       public:
        ImageCopyTexture(const Texture &texture);
        ~ImageCopyTexture() = default;

        inline static ImageCopyTexture New(const Texture &texture) { return ImageCopyTexture{texture}; }

        void write(const void *data) const;

        ImageCopyTexture &setMipLevel(std::uint32_t mipLevel);
        ImageCopyTexture &setOrigin3D(std::uint32_t x, std::uint32_t y, std::uint32_t z);
        ImageCopyTexture &setAspect(TextureAspect aspect);

        const WGPUImageCopyTexture &getRef() const;
        const Texture &getTextureRef() const;

       private:
        Texture m_Texture;
        WGPUImageCopyTexture m_ImageCopyTexture;
    };

    class CommandBuffer {
       public:
        CommandBuffer(const WGPUCommandBuffer &handle);

        CommandBuffer();
        ~CommandBuffer();

        CommandBuffer(const CommandBuffer &other);
        CommandBuffer(CommandBuffer &&other);

        CommandBuffer &operator=(const CommandBuffer &other);
        CommandBuffer &operator=(CommandBuffer &&other);

        WGPUCommandBuffer get() const;

       private:
        WGPUCommandBuffer m_CommandBuffer;
    };

    class CommandEncoder {
       public:
        CommandEncoder(const std::string &label, const WGPUCommandEncoder &handle);

        CommandEncoder();
        ~CommandEncoder();

        CommandEncoder(const CommandEncoder &other);
        CommandEncoder(CommandEncoder &&other);

        CommandEncoder &operator=(const CommandEncoder &other);
        CommandEncoder &operator=(CommandEncoder &&other);

        void copyTextureToTexture(const ImageCopyTexture &src, const ImageCopyTexture &dst);
        void copyBufferToBuffer(const Buffer &src, const Buffer &dst);
        void copyTextureToBuffer(const ImageCopyTexture &src, const Buffer &dst);
        void copyBufferToTexture(const Buffer &src, const ImageCopyTexture &dst);

        WGPUCommandEncoder get() const;
        std::string getLabel() const;

        CommandBuffer finish(const std::string &label = "Yulduz Command Buffer");

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

        CommandEncoder build(const GraphicsContext &context);

       private:
        std::string m_Label;
    };
}  // namespace Yulduz
