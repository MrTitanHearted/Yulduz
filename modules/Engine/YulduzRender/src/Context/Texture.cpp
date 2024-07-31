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

    void Texture::resize2D(std::uint32_t width, std::uint32_t height, const std::shared_ptr<RenderContext> &context) {
        if ((getWidth() == width && getHeight() == height) || width == 0 || height == 0) return;

        WGPUTextureDescriptor descriptor{
            .usage = static_cast<WGPUTextureUsageFlags>(getUsage()),
            .dimension = static_cast<WGPUTextureDimension>(getDimension()),
            .size = WGPUExtent3D{.width = width, .height = height, .depthOrArrayLayers = 1},
            .format = static_cast<WGPUTextureFormat>(getFormat()),
            .mipLevelCount = 1,
            .sampleCount = 1,
            .viewFormatCount = 0,
            .viewFormats = nullptr,
        };
        WGPUTexture texture = wgpuDeviceCreateTexture(context->getDevice(), &descriptor);
        WGPUTextureView view = wgpuTextureCreateView(texture, nullptr);

        wgpuTextureViewRelease(m_View);
        wgpuTextureRelease(m_Texture);

        m_Texture = texture;
        m_View = view;
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

    std::array<std::uint32_t, 2> Texture::getSize2D() const {
        std::uint32_t width = wgpuTextureGetWidth(m_Texture);
        std::uint32_t height = wgpuTextureGetHeight(m_Texture);
        return {width, height};
    }

    std::array<std::uint32_t, 3> Texture::getSize3D() const {
        std::uint32_t width = wgpuTextureGetWidth(m_Texture);
        std::uint32_t height = wgpuTextureGetHeight(m_Texture);
        std::uint32_t depthOrArrayLayers = wgpuTextureGetDepthOrArrayLayers(m_Texture);
        return {width, height, depthOrArrayLayers};
    }

    std::uint32_t Texture::getWidth() const {
        return wgpuTextureGetWidth(m_Texture);
    }

    std::uint32_t Texture::getHeight() const {
        return wgpuTextureGetHeight(m_Texture);
    }

    std::uint32_t Texture::getDepthOrArrayLayers() const {
        return wgpuTextureGetDepthOrArrayLayers(m_Texture);
    }

    std::uint32_t Texture::getFormatSize() const {
        return FormatSize(getFormat());
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

    TextureBuilder &TextureBuilder::setMipLevelCount(std::uint32_t mipLevelCount) {
        m_Descriptor.mipLevelCount = mipLevelCount;
        return *this;
    }

    TextureBuilder &TextureBuilder::setSampleCount(std::uint32_t sampleCount) {
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

    std::shared_ptr<Texture> TextureBuilder::empty3d(std::uint32_t width, std::uint32_t height, std::uint32_t depthOrArrayLayers, const std::shared_ptr<RenderContext> &context) {
        return empty(width, height, depthOrArrayLayers, context);
    }

    std::shared_ptr<Texture> TextureBuilder::empty2d(std::uint32_t width, std::uint32_t height, const std::shared_ptr<RenderContext> &context) {
        return empty(width, height, 1, context);
    }

    std::shared_ptr<Texture> TextureBuilder::build(const std::shared_ptr<TextureAsset> &asset, const std::shared_ptr<RenderContext> &context) {
        if (asset->getData() == nullptr || asset->getSize() == 0) {
            YZERROR("Texture Asset: '{}' is empty", asset->getPath().string());
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

    std::shared_ptr<Framebuffer> TextureBuilder::emptyFramebuffer(std::uint32_t width, std::uint32_t height, const std::shared_ptr<RenderContext> &context) {
        m_Descriptor.label = m_Label.c_str();
        m_Descriptor.usage |= WGPUTextureUsage_RenderAttachment;
        m_Descriptor.viewFormatCount = m_ViewFormats.size();
        m_Descriptor.viewFormats = m_ViewFormats.data();
        m_Descriptor.size = WGPUExtent3D{.width = width, .height = height, .depthOrArrayLayers = 1};
        m_Descriptor.dimension = WGPUTextureDimension_2D;

        WGPUTexture texture = wgpuDeviceCreateTexture(context->getDevice(), &m_Descriptor);

        return std::make_shared<Framebuffer>(m_Label, texture);
    }

    std::shared_ptr<Texture> TextureBuilder::empty(std::uint32_t width, std::uint32_t height, std::uint32_t depthOrArrayLayers, const std::shared_ptr<RenderContext> &context) {
        m_Descriptor.label = m_Label.c_str();
        m_Descriptor.viewFormatCount = m_ViewFormats.size();
        m_Descriptor.viewFormats = m_ViewFormats.data();
        m_Descriptor.size = WGPUExtent3D{.width = width, .height = height, .depthOrArrayLayers = depthOrArrayLayers};
        m_Descriptor.dimension = depthOrArrayLayers <= 1 ? WGPUTextureDimension_2D : WGPUTextureDimension_3D;

        WGPUTexture texture = wgpuDeviceCreateTexture(context->getDevice(), &m_Descriptor);

        return std::make_shared<Texture>(m_Label, texture);
    }

    std::uint32_t Texture::FormatSize(TextureFormat format) {
        static const std::unordered_map<TextureFormat, size_t> formatSizes = {
            {TextureFormat::Undefined, 0},
            {TextureFormat::R8Unorm, 1},
            {TextureFormat::R8Snorm, 1},
            {TextureFormat::R8Uint, 1},
            {TextureFormat::R8Sint, 1},
            {TextureFormat::R16Uint, 2},
            {TextureFormat::R16Sint, 2},
            {TextureFormat::R16Float, 2},
            {TextureFormat::RG8Unorm, 2},
            {TextureFormat::RG8Snorm, 2},
            {TextureFormat::RG8Uint, 2},
            {TextureFormat::RG8Sint, 2},
            {TextureFormat::R32Float, 4},
            {TextureFormat::R32Uint, 4},
            {TextureFormat::R32Sint, 4},
            {TextureFormat::RG16Uint, 4},
            {TextureFormat::RG16Sint, 4},
            {TextureFormat::RG16Float, 4},
            {TextureFormat::RGBA8Unorm, 4},
            {TextureFormat::RGBA8UnormSrgb, 4},
            {TextureFormat::RGBA8Snorm, 4},
            {TextureFormat::RGBA8Uint, 4},
            {TextureFormat::RGBA8Sint, 4},
            {TextureFormat::BGRA8Unorm, 4},
            {TextureFormat::BGRA8UnormSrgb, 4},
            {TextureFormat::RGB10A2Uint, 4},
            {TextureFormat::RGB10A2Unorm, 4},
            {TextureFormat::RG11B10Ufloat, 4},
            {TextureFormat::RGB9E5Ufloat, 4},
            {TextureFormat::RG32Float, 8},
            {TextureFormat::RG32Uint, 8},
            {TextureFormat::RG32Sint, 8},
            {TextureFormat::RGBA16Uint, 8},
            {TextureFormat::RGBA16Sint, 8},
            {TextureFormat::RGBA16Float, 8},
            {TextureFormat::RGBA32Float, 16},
            {TextureFormat::RGBA32Uint, 16},
            {TextureFormat::RGBA32Sint, 16},
            {TextureFormat::Stencil8, 1},
            {TextureFormat::Depth16Unorm, 2},
            {TextureFormat::Depth24Plus, 3},  // Approximate size
            {TextureFormat::Depth24PlusStencil8, 4},
            {TextureFormat::Depth32Float, 4},
            {TextureFormat::Depth32FloatStencil8, 5},  // Approximate size
            {TextureFormat::BC1RGBAUnorm, 8},          // Approximate size per 4x4 block
            {TextureFormat::BC1RGBAUnormSrgb, 8},
            {TextureFormat::BC2RGBAUnorm, 16},
            {TextureFormat::BC2RGBAUnormSrgb, 16},
            {TextureFormat::BC3RGBAUnorm, 16},
            {TextureFormat::BC3RGBAUnormSrgb, 16},
            {TextureFormat::BC4RUnorm, 8},
            {TextureFormat::BC4RSnorm, 8},
            {TextureFormat::BC5RGUnorm, 16},
            {TextureFormat::BC5RGSnorm, 16},
            {TextureFormat::BC6HRGBUfloat, 16},
            {TextureFormat::BC6HRGBFloat, 16},
            {TextureFormat::BC7RGBAUnorm, 16},
            {TextureFormat::BC7RGBAUnormSrgb, 16},
            {TextureFormat::ETC2RGB8Unorm, 8},
            {TextureFormat::ETC2RGB8UnormSrgb, 8},
            {TextureFormat::ETC2RGB8A1Unorm, 8},
            {TextureFormat::ETC2RGB8A1UnormSrgb, 8},
            {TextureFormat::ETC2RGBA8Unorm, 16},
            {TextureFormat::ETC2RGBA8UnormSrgb, 16},
            {TextureFormat::EACR11Unorm, 8},
            {TextureFormat::EACR11Snorm, 8},
            {TextureFormat::EACRG11Unorm, 16},
            {TextureFormat::EACRG11Snorm, 16},
            {TextureFormat::ASTC4x4Unorm, 16},
            {TextureFormat::ASTC4x4UnormSrgb, 16},
            {TextureFormat::ASTC5x4Unorm, 16},
            {TextureFormat::ASTC5x4UnormSrgb, 16},
            {TextureFormat::ASTC5x5Unorm, 16},
            {TextureFormat::ASTC5x5UnormSrgb, 16},
            {TextureFormat::ASTC6x5Unorm, 16},
            {TextureFormat::ASTC6x5UnormSrgb, 16},
            {TextureFormat::ASTC6x6Unorm, 16},
            {TextureFormat::ASTC6x6UnormSrgb, 16},
            {TextureFormat::ASTC8x5Unorm, 16},
            {TextureFormat::ASTC8x5UnormSrgb, 16},
            {TextureFormat::ASTC8x6Unorm, 16},
            {TextureFormat::ASTC8x6UnormSrgb, 16},
            {TextureFormat::ASTC8x8Unorm, 16},
            {TextureFormat::ASTC8x8UnormSrgb, 16},
            {TextureFormat::ASTC10x5Unorm, 16},
            {TextureFormat::ASTC10x5UnormSrgb, 16},
            {TextureFormat::ASTC10x6Unorm, 16},
            {TextureFormat::ASTC10x6UnormSrgb, 16},
            {TextureFormat::ASTC10x8Unorm, 16},
            {TextureFormat::ASTC10x8UnormSrgb, 16},
            {TextureFormat::ASTC10x10Unorm, 16},
            {TextureFormat::ASTC10x10UnormSrgb, 16},
            {TextureFormat::ASTC12x10Unorm, 16},
            {TextureFormat::ASTC12x10UnormSrgb, 16},
            {TextureFormat::ASTC12x12Unorm, 16},
            {TextureFormat::ASTC12x12UnormSrgb, 16},
            {TextureFormat::Force32, 4},
        };

        auto it = formatSizes.find(format);
        if (it != formatSizes.end()) {
            return it->second;
        } else {
            return 0;
        }
    }
}  // namespace Yulduz
