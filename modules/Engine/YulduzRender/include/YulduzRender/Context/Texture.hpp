#pragma once

#include <Yulduz/Core.hpp>
#include <Yulduz/Assets.hpp>
#include <YulduzRender/Enums.hpp>

namespace Yulduz {
    class RenderContext;

    class Texture {
       public:
        Texture(const std::string &label, const WGPUTexture &texture);
        ~Texture();

        std::string getLabel() const;
        WGPUTexture get() const;
        WGPUTextureView getView() const;

        TextureFormat getFormat() const;
        TextureDimension getDimension() const;
        TextureUsage getUsage() const;
        std::array<uint32_t, 2> getSize2D() const;
        std::array<uint32_t, 3> getSize3D() const;
        uint32_t getWidth() const;
        uint32_t getHeight() const;
        uint32_t getDepthOrArrayLayers() const;

       private:
        std::string m_Label;
        WGPUTexture m_Texture;
        WGPUTextureView m_View;
        bool m_IsDefaultTexture;

       private:
        Texture() = default;

        friend class Framebuffer;
    };

    class Framebuffer : public Texture {
       public:
        Framebuffer(const std::string &label, const WGPUTexture &texture);
        ~Framebuffer();
    };

    class TextureBuilder {
       public:
        TextureBuilder();
        ~TextureBuilder() = default;

        inline static TextureBuilder New() { return TextureBuilder(); }

        TextureBuilder &setLabel(const std::string &label);
        TextureBuilder &setTextureUsage(TextureUsage usage);
        TextureBuilder &addTextureUsage(TextureUsage usage);
        TextureBuilder &setFormat(TextureFormat format);
        TextureBuilder &setMipLevelCount(uint32_t mipLevelCount);
        TextureBuilder &setSampleCount(uint32_t sampleCount);
        TextureBuilder &setViewFormats(const std::vector<TextureFormat> &viewFormats);
        TextureBuilder &addViewFormat(TextureFormat viewFormat);

        std::shared_ptr<Texture> empty3d(uint32_t width, uint32_t height, uint32_t depthOrArrayLayers, const std::shared_ptr<RenderContext> &context);
        std::shared_ptr<Texture> empty2d(uint32_t width, uint32_t height, const std::shared_ptr<RenderContext> &context);
        std::shared_ptr<Texture> build(const std::shared_ptr<TextureAsset> &asset, const std::shared_ptr<RenderContext> &context);

        std::shared_ptr<Framebuffer> emptyFramebuffer(uint32_t width, uint32_t height, const std::shared_ptr<RenderContext> &context);

       private:
        std::string m_Label;
        WGPUTextureDescriptor m_Descriptor;
        std::vector<WGPUTextureFormat> m_ViewFormats;

        std::shared_ptr<Texture> empty(uint32_t width, uint32_t height, uint32_t depthOrArrayLayers, const std::shared_ptr<RenderContext> &context);
    };
}  // namespace Yulduz
