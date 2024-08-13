#pragma once

#include <Yulduz/Core.hpp>
#include <YulduzGraphics/Context/GraphicsContext.hpp>
#include <YulduzGraphics/Enums.hpp>

namespace Yulduz {
    class TextureAsset;

    class Texture {
       public:
        Texture(const std::string &label, const WGPUTexture &handle, const GraphicsContext &context);
        Texture() = default;
        ~Texture();

        Texture(const Texture &other);
        Texture &operator=(const Texture &other);

        Texture(Texture &&other);
        Texture &operator=(Texture &&other);

        void resize2D(std::uint32_t width, std::uint32_t height);
        void resize3D(std::uint32_t width, std::uint32_t height, std::uint32_t depthOrArrayLayers);
        void resize2D(std::uint32_t width, std::uint32_t height, const std::vector<TextureFormat> &viewFormats);
        void resize3D(std::uint32_t width, std::uint32_t height, std::uint32_t depthOrArrayLayers, const std::vector<TextureFormat> &viewFormats);

        WGPUTexture get() const;
        WGPUTextureView getDefaultView() const;
        const GraphicsContext &getContextRef() const;
        GraphicsContext &getContextMut();
        std::string getLabel() const;

        TextureFormat getFormat() const;
        TextureDimension getDimension() const;
        TextureUsage getUsage() const;
        std::array<std::uint32_t, 2> getSize2D() const;
        std::array<std::uint32_t, 3> getSize3D() const;
        std::uint32_t getWidth() const;
        std::uint32_t getHeight() const;
        std::uint32_t getDepthOrArrayLayers() const;
        std::uint32_t getMipLevelCount() const;
        std::uint32_t getSampleCount() const;
        std::uint32_t getFormatSize() const;

       private:
        std::string m_Label;
        WGPUTexture m_Texture;
        WGPUTextureView m_DefaultView;

        GraphicsContext m_ContextReference;
    };

    class TextureView {
       public:
        TextureView(const std::string &label, const WGPUTextureView &handle, const Texture &texture);
        ~TextureView();

        TextureView(const TextureView &other);
        TextureView &operator=(const TextureView &other);

        TextureView(TextureView &&other);
        TextureView &operator=(TextureView &&other);

        WGPUTextureView get() const;
        const Texture &getTextureRef() const;
        Texture &getTextureMut();
        std::string getLabel() const;

       private:
        std::string m_Label;
        WGPUTextureView m_View;

        Texture m_TextureReference;
    };

    class TextureBuilder {
       public:
        TextureBuilder();
        ~TextureBuilder() = default;

        inline static TextureBuilder New() { return TextureBuilder(); }

        TextureBuilder &setLabel(const std::string &label);
        TextureBuilder &setUsage(TextureUsage usage);
        TextureBuilder &addUsage(TextureUsage usage);
        TextureBuilder &setFormat(TextureFormat format);
        TextureBuilder &setMipLevelCount(std::uint32_t mipLevelCount);
        TextureBuilder &setSampleCount(std::uint32_t sampleCount);
        TextureBuilder &setViewFormats(const std::vector<TextureFormat> &viewFormats);
        TextureBuilder &addViewFormat(TextureFormat viewFormat);

        Texture empty3d(std::uint32_t width, std::uint32_t height, std::uint32_t depthOrArrayLayers, const GraphicsContext &context);
        Texture empty2d(std::uint32_t width, std::uint32_t height, const GraphicsContext &context);
        Texture build(const std::shared_ptr<TextureAsset> &asset, const GraphicsContext &context);

       private:
        std::string m_Label;
        WGPUTextureDescriptor m_Descriptor;
        std::vector<TextureFormat> m_ViewFormats;

       private:
        Texture empty(std::uint32_t width, std::uint32_t height, std::uint32_t depthOrArrayLayers, const GraphicsContext &context);
    };

    class TextureViewBuilder {
       public:
        TextureViewBuilder();
        ~TextureViewBuilder() = default;

        inline static TextureViewBuilder New() { return TextureViewBuilder(); }

        TextureViewBuilder &setLabel(const std::string &label);
        TextureViewBuilder &setFormat(TextureFormat format);
        TextureViewBuilder &setDimension(TextureViewDimension dimension);
        TextureViewBuilder &setBaseMipLevel(std::uint32_t baseMipLevel);
        TextureViewBuilder &setBaseArrayLayer(std::uint32_t baseArrayLayer);
        TextureViewBuilder &setAspect(TextureAspect aspect);

        TextureView build(const Texture &texture);

       private:
        std::string m_Label;
        WGPUTextureViewDescriptor m_Descriptor;
    };
}  // namespace Yulduz
