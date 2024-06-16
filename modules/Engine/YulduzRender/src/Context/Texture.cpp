#include <YulduzRender/Context/Texture.hpp>
#include <YulduzRender/Context.hpp>

namespace Yulduz {
    Texture::Texture(const std::string &label, const WGPUTexture &texture) {
        YZDEBUG("Initializing Texture: '{}'", label);

        m_Label = label;
        m_Texture = texture;
        m_View = wgpuTextureCreateView(texture, nullptr);
        m_IsDefaultTexture = true;
    }

    Texture::~Texture() {
        if (!m_IsDefaultTexture) return;
        YZDEBUG("Releasing Texture: '{}'", m_Label);

        wgpuTextureViewRelease(m_View);
        wgpuTextureRelease(m_Texture);
    }

    std::string Texture::getLabel() const {
        return m_Label;
    }

    WGPUTexture Texture::get() const {
        return m_Texture;
    }

    WGPUTextureView Texture::getView() const {
        return m_View;
    }

    TextureFormat Texture::getFormat() const {
        return static_cast<TextureFormat>(wgpuTextureGetFormat(m_Texture));
    }

    TextureDimension Texture::getDimension() const {
        return static_cast<TextureDimension>(wgpuTextureGetDimension(m_Texture));
    }

    TextureUsage Texture::getUsage() const {
        return static_cast<TextureUsage>(wgpuTextureGetUsage(m_Texture));
    }

    std::array<uint32_t, 2> Texture::getSize2D() const {
        uint32_t width = wgpuTextureGetWidth(m_Texture);
        uint32_t height = wgpuTextureGetHeight(m_Texture);
        return {width, height};
    }

    std::array<uint32_t, 3> Texture::getSize3D() const {
        uint32_t width = wgpuTextureGetWidth(m_Texture);
        uint32_t height = wgpuTextureGetHeight(m_Texture);
        uint32_t depthOrArrayLayers = wgpuTextureGetDepthOrArrayLayers(m_Texture);
        return {width, height, depthOrArrayLayers};
    }

    uint32_t Texture::getWidth() const {
        return wgpuTextureGetWidth(m_Texture);
    }

    uint32_t Texture::getHeight() const {
        return wgpuTextureGetHeight(m_Texture);
    }

    uint32_t Texture::getDepthOrArrayLayers() const {
        return wgpuTextureGetDepthOrArrayLayers(m_Texture);
    }

    Framebuffer::Framebuffer(const std::string &label, const WGPUTexture &texture) {
        YZINFO("Initializing Framebuffer: '{}'", label);

        m_Label = label;
        m_Texture = texture;
        m_View = wgpuTextureCreateView(texture, nullptr);
        m_IsDefaultTexture = false;
    }

    Framebuffer::~Framebuffer() {
        YZINFO("Releasing Framebuffer: '{}'", m_Label);

        wgpuTextureViewRelease(m_View);
        wgpuTextureRelease(m_Texture);
    }

    TextureBuilder::TextureBuilder() {
        m_Label = "Yulduz Texture";
        m_Descriptor = WGPUTextureDescriptor{
            .usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst,
            .format = WGPUTextureFormat_RGBA8Unorm,
            .mipLevelCount = 1,
            .sampleCount = 1,
        };
    }

    TextureBuilder &TextureBuilder::setLabel(const std::string &label) {
        m_Label = label;
        return *this;
    }

    TextureBuilder &TextureBuilder::setTextureUsage(TextureUsage usage) {
        m_Descriptor.usage = static_cast<WGPUTextureUsage>(usage);
        return *this;
    }

    TextureBuilder &TextureBuilder::addTextureUsage(TextureUsage usage) {
        m_Descriptor.usage |= static_cast<WGPUTextureUsage>(usage);
        return *this;
    }

    TextureBuilder &TextureBuilder::setFormat(TextureFormat format) {
        m_Descriptor.format = static_cast<WGPUTextureFormat>(format);
        return *this;
    }

    TextureBuilder &TextureBuilder::setMipLevelCount(uint32_t mipLevelCount) {
        m_Descriptor.mipLevelCount = mipLevelCount;
        return *this;
    }

    TextureBuilder &TextureBuilder::setSampleCount(uint32_t sampleCount) {
        m_Descriptor.sampleCount = sampleCount;
        return *this;
    }

    TextureBuilder &TextureBuilder::setViewFormats(const std::vector<TextureFormat> &viewFormats) {
        m_ViewFormats.clear();
        m_ViewFormats.reserve(viewFormats.size());
        for (const TextureFormat &format : viewFormats) {
            m_ViewFormats.emplace_back(static_cast<WGPUTextureFormat>(format));
        }
        return *this;
    }

    TextureBuilder &TextureBuilder::addViewFormat(TextureFormat viewFormat) {
        m_ViewFormats.emplace_back(static_cast<WGPUTextureFormat>(viewFormat));
        return *this;
    }

    std::shared_ptr<Texture> TextureBuilder::empty3d(uint32_t width, uint32_t height, uint32_t depthOrArrayLayers, const std::shared_ptr<RenderContext> &context) {
        return empty(width, height, depthOrArrayLayers, context);
    }

    std::shared_ptr<Texture> TextureBuilder::empty2d(uint32_t width, uint32_t height, const std::shared_ptr<RenderContext> &context) {
        return empty(width, height, 1, context);
    }

    std::shared_ptr<Texture> TextureBuilder::build(const std::shared_ptr<TextureAsset> &asset, const std::shared_ptr<RenderContext> &context) {
        if (asset->getData() == nullptr || asset->getSize() == 0) {
            YZERROR("Texture Asset: '{}' is empty", asset->getPath());
            return nullptr;
        }

        std::shared_ptr<Texture> texture = empty(asset->getWidth(), asset->getHeight(), 1, context);

        WGPUImageCopyTexture destination{
            .texture = texture->get(),
            .mipLevel = 0,
            .origin = WGPUOrigin3D{.x = 0, .y = 0, .z = 0},
            .aspect = WGPUTextureAspect_All,
        };

        WGPUTextureDataLayout dataLayout{
            .offset = 0,
            .bytesPerRow = asset->getStride(),
            .rowsPerImage = asset->getHeight(),
        };

        wgpuQueueWriteTexture(context->getQueue(), &destination, asset->getData(), asset->getSize(), &dataLayout, &m_Descriptor.size);

        return texture;
    }

    std::shared_ptr<Framebuffer> TextureBuilder::emptyFramebuffer(uint32_t width, uint32_t height, const std::shared_ptr<RenderContext> &context) {
        m_Descriptor.label = m_Label.c_str();
        m_Descriptor.usage |= WGPUTextureUsage_RenderAttachment;
        m_Descriptor.viewFormatCount = m_ViewFormats.size();
        m_Descriptor.viewFormats = m_ViewFormats.data();
        m_Descriptor.size = WGPUExtent3D{.width = width, .height = height, .depthOrArrayLayers = 1};
        m_Descriptor.dimension = WGPUTextureDimension_2D;

        WGPUTexture texture = wgpuDeviceCreateTexture(context->getDevice(), &m_Descriptor);

        return std::make_shared<Framebuffer>(m_Label, texture);
    }

    std::shared_ptr<Texture> TextureBuilder::empty(uint32_t width, uint32_t height, uint32_t depthOrArrayLayers, const std::shared_ptr<RenderContext> &context) {
        m_Descriptor.label = m_Label.c_str();
        m_Descriptor.viewFormatCount = m_ViewFormats.size();
        m_Descriptor.viewFormats = m_ViewFormats.data();
        m_Descriptor.size = WGPUExtent3D{.width = width, .height = height, .depthOrArrayLayers = depthOrArrayLayers};
        m_Descriptor.dimension = depthOrArrayLayers <= 1 ? WGPUTextureDimension_2D : WGPUTextureDimension_3D;

        WGPUTexture texture = wgpuDeviceCreateTexture(context->getDevice(), &m_Descriptor);

        return std::make_shared<Texture>(m_Label, texture);
    }
}  // namespace Yulduz
