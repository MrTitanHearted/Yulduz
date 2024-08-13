#include <Yulduz/Assets.hpp>
#include <YulduzGraphics/Context/GraphicsContext.hpp>
#include <YulduzGraphics/Context/Texture.hpp>

namespace Yulduz {
    Texture::Texture(const std::string &label, const WGPUTexture &handle, const GraphicsContext &context) : m_ContextReference{context} {
        assert(handle != nullptr && "Texture handle cannot be nullptr");

        m_Texture = handle;
        m_DefaultView = wgpuTextureCreateView(m_Texture, nullptr);
        m_Label = label;
    }

    Texture::~Texture() {
        if (m_Texture) {
            wgpuTextureViewRelease(m_DefaultView);
            wgpuTextureRelease(m_Texture);
        }
    }

    Texture::Texture(const Texture &other) : m_ContextReference{other.m_ContextReference} {
        assert(other.m_Texture != nullptr && "Texture handle cannot be nullptr");

        wgpuTextureViewReference(other.m_DefaultView);
        wgpuTextureReference(other.m_Texture);

        m_DefaultView = other.m_DefaultView;
        m_Texture = other.m_Texture;
        m_Label = other.m_Label;
    }

    Texture &Texture::operator=(const Texture &other) {
        assert(other.m_Texture != nullptr && "Texture handle cannot be nullptr");

        if (&other != this) {
            wgpuTextureViewRelease(m_DefaultView);
            wgpuTextureRelease(m_Texture);

            m_ContextReference = other.m_ContextReference;
            m_DefaultView = other.m_DefaultView;
            m_Texture = other.m_Texture;
            m_Label = other.m_Label;

            wgpuTextureViewReference(m_DefaultView);
            wgpuTextureReference(m_Texture);
        }

        return *this;
    }

    Texture::Texture(Texture &&other) : m_ContextReference{other.m_ContextReference} {
        assert(other.m_Texture != nullptr && "Texture handle cannot be nullptr");

        m_DefaultView = other.m_DefaultView;
        m_Texture = other.m_Texture;
        m_Label = other.m_Label;

        other.m_DefaultView = nullptr;
        other.m_Texture = nullptr;
        other.m_Label = "";
        other.m_ContextReference = {};
    }

    Texture &Texture::operator=(Texture &&other) {
        assert(other.m_Texture != nullptr && "Texture handle cannot be nullptr");

        if (&other != this) {
            wgpuTextureViewRelease(m_DefaultView);
            wgpuTextureRelease(m_Texture);

            m_ContextReference = other.m_ContextReference;
            m_DefaultView = other.m_DefaultView;
            m_Texture = other.m_Texture;
            m_Label = other.m_Label;

            other.m_DefaultView = nullptr;
            other.m_Texture = nullptr;
            other.m_Label = "";
            other.m_ContextReference = {};
        }

        return *this;
    }

    void Texture::resize2D(std::uint32_t width, std::uint32_t height) {
        assert(m_Texture != nullptr && "Texture handle cannot be nullptr");

        if ((getWidth() == width && getHeight() == height) || width == 0 || height == 0) return;

        WGPUTextureDescriptor descriptor{
            .label = m_Label.c_str(),
            .usage = static_cast<WGPUTextureUsageFlags>(getUsage()),
            .dimension = static_cast<WGPUTextureDimension>(getDimension()),
            .size = WGPUExtent3D{.width = width, .height = height, .depthOrArrayLayers = 1},
            .format = static_cast<WGPUTextureFormat>(getFormat()),
            .mipLevelCount = getMipLevelCount(),
            .sampleCount = getSampleCount(),
            .viewFormatCount = 0,
            .viewFormats = nullptr,
        };

        wgpuTextureViewRelease(m_DefaultView);
        wgpuTextureRelease(m_Texture);

        m_Texture = wgpuDeviceCreateTexture(m_ContextReference.getDevice(), &descriptor);
        m_DefaultView = wgpuTextureCreateView(m_Texture, nullptr);
    }

    void Texture::resize3D(std::uint32_t width, std::uint32_t height, std::uint32_t depthOrArrayLayers) {
        assert(m_Texture != nullptr && "Texture handle cannot be nullptr");

        if ((getWidth() == width && getHeight() == height) || width == 0 || height == 0) return;

        WGPUTextureDescriptor descriptor{
            .label = m_Label.c_str(),
            .usage = static_cast<WGPUTextureUsageFlags>(getUsage()),
            .dimension = static_cast<WGPUTextureDimension>(getDimension()),
            .size = WGPUExtent3D{.width = width, .height = height, .depthOrArrayLayers = depthOrArrayLayers},
            .format = static_cast<WGPUTextureFormat>(getFormat()),
            .mipLevelCount = getMipLevelCount(),
            .sampleCount = getSampleCount(),
            .viewFormatCount = 0,
            .viewFormats = nullptr,
        };

        wgpuTextureViewRelease(m_DefaultView);
        wgpuTextureRelease(m_Texture);

        m_Texture = wgpuDeviceCreateTexture(m_ContextReference.getDevice(), &descriptor);
        m_DefaultView = wgpuTextureCreateView(m_Texture, nullptr);
    }

    void Texture::resize2D(std::uint32_t width, std::uint32_t height, const std::vector<TextureFormat> &viewFormats) {
        assert(m_Texture != nullptr && "Texture handle cannot be nullptr");

        if ((getWidth() == width && getHeight() == height) || width == 0 || height == 0) return;

        WGPUTextureDescriptor descriptor{
            .label = m_Label.c_str(),
            .usage = static_cast<WGPUTextureUsageFlags>(getUsage()),
            .dimension = static_cast<WGPUTextureDimension>(getDimension()),
            .size = WGPUExtent3D{.width = width, .height = height, .depthOrArrayLayers = 1},
            .format = static_cast<WGPUTextureFormat>(getFormat()),
            .mipLevelCount = getMipLevelCount(),
            .sampleCount = getSampleCount(),
            .viewFormatCount = viewFormats.size(),
            .viewFormats = (WGPUTextureFormat *)viewFormats.data(),
        };

        wgpuTextureViewRelease(m_DefaultView);
        wgpuTextureRelease(m_Texture);

        m_Texture = wgpuDeviceCreateTexture(m_ContextReference.getDevice(), &descriptor);
        m_DefaultView = wgpuTextureCreateView(m_Texture, nullptr);
    }

    void Texture::resize3D(std::uint32_t width, std::uint32_t height, std::uint32_t depthOrArrayLayers, const std::vector<TextureFormat> &viewFormats) {
        assert(m_Texture != nullptr && "Texture handle cannot be nullptr");

        if ((getWidth() == width && getHeight() == height) || width == 0 || height == 0) return;

        WGPUTextureDescriptor descriptor{
            .label = m_Label.c_str(),
            .usage = static_cast<WGPUTextureUsageFlags>(getUsage()),
            .dimension = static_cast<WGPUTextureDimension>(getDimension()),
            .size = WGPUExtent3D{.width = width, .height = height, .depthOrArrayLayers = depthOrArrayLayers},
            .format = static_cast<WGPUTextureFormat>(getFormat()),
            .mipLevelCount = getMipLevelCount(),
            .sampleCount = getSampleCount(),
            .viewFormatCount = viewFormats.size(),
            .viewFormats = (WGPUTextureFormat *)viewFormats.data(),
        };

        wgpuTextureViewRelease(m_DefaultView);
        wgpuTextureRelease(m_Texture);

        m_Texture = wgpuDeviceCreateTexture(m_ContextReference.getDevice(), &descriptor);
        m_DefaultView = wgpuTextureCreateView(m_Texture, nullptr);
    }

    WGPUTexture Texture::get() const {
        assert(m_Texture != nullptr && "Texture handle cannot be nullptr");

        return m_Texture;
    }

    WGPUTextureView Texture::getDefaultView() const {
        assert(m_Texture != nullptr && "Texture handle cannot be nullptr");

        return m_DefaultView;
    }

    const GraphicsContext &Texture::getContextRef() const {
        assert(m_Texture != nullptr && "Texture handle cannot be nullptr");

        return m_ContextReference;
    }

    GraphicsContext &Texture::getContextMut() {
        assert(m_Texture != nullptr && "Texture handle cannot be nullptr");

        return m_ContextReference;
    }

    TextureFormat Texture::getFormat() const {
        assert(m_Texture != nullptr && "Texture handle cannot be nullptr");

        return static_cast<TextureFormat>(wgpuTextureGetFormat(m_Texture));
    }

    TextureDimension Texture::getDimension() const {
        assert(m_Texture != nullptr && "Texture handle cannot be nullptr");

        return static_cast<TextureDimension>(wgpuTextureGetDimension(m_Texture));
    }

    TextureUsage Texture::getUsage() const {
        assert(m_Texture != nullptr && "Texture handle cannot be nullptr");

        return static_cast<TextureUsage>(wgpuTextureGetUsage(m_Texture));
    }

    std::array<std::uint32_t, 2> Texture::getSize2D() const {
        assert(m_Texture != nullptr && "Texture handle cannot be nullptr");

        return {wgpuTextureGetWidth(m_Texture), wgpuTextureGetHeight(m_Texture)};
    }

    std::array<std::uint32_t, 3> Texture::getSize3D() const {
        assert(m_Texture != nullptr && "Texture handle cannot be nullptr");

        return {wgpuTextureGetWidth(m_Texture), wgpuTextureGetHeight(m_Texture), wgpuTextureGetDepthOrArrayLayers(m_Texture)};
    }

    std::uint32_t Texture::getWidth() const {
        assert(m_Texture != nullptr && "Texture handle cannot be nullptr");

        return wgpuTextureGetWidth(m_Texture);
    }

    std::uint32_t Texture::getHeight() const {
        assert(m_Texture != nullptr && "Texture handle cannot be nullptr");

        return wgpuTextureGetHeight(m_Texture);
    }

    std::uint32_t Texture::getDepthOrArrayLayers() const {
        assert(m_Texture != nullptr && "Texture handle cannot be nullptr");

        return wgpuTextureGetDepthOrArrayLayers(m_Texture);
    }

    std::uint32_t Texture::getMipLevelCount() const {
        assert(m_Texture != nullptr && "Texture handle cannot be nullptr");

        return wgpuTextureGetMipLevelCount(m_Texture);
    }

    std::uint32_t Texture::getSampleCount() const {
        assert(m_Texture != nullptr && "Texture handle cannot be nullptr");

        return wgpuTextureGetSampleCount(m_Texture);
    }

    std::uint32_t Texture::getFormatSize() const {
        assert(m_Texture != nullptr && "Texture handle cannot be nullptr");

        return GetTextureFormatSize(static_cast<TextureFormat>(wgpuTextureGetFormat(m_Texture)));
    }

    TextureView::TextureView(const std::string &label,const WGPUTextureView &handle, const Texture &texture) : m_TextureReference{texture} {
        assert(handle != nullptr && "Texture View handle cannot be nullptr");

        m_View = handle;
        m_Label = label;
    }

    TextureView::~TextureView() {
        if (m_View) {
            wgpuTextureViewRelease(m_View);
        }
    }

    TextureView::TextureView(const TextureView &other) : m_TextureReference{other.m_TextureReference} {
        assert(other.m_View != nullptr && "Texture View handle cannot be nullptr");

        wgpuTextureViewReference(other.m_View);

        m_View = other.m_View;
        m_Label = other.m_Label;
    }

    TextureView &TextureView::operator=(const TextureView &other) {
        assert(other.m_View != nullptr && "Texture View handle cannot be nullptr");

        if (&other != this) {
            wgpuTextureViewRelease(m_View);

            m_TextureReference = other.m_TextureReference;
            m_View = other.m_View;
            m_Label = other.m_Label;

            wgpuTextureViewReference(m_View);
        }

        return *this;
    }

    TextureView::TextureView(TextureView &&other) : m_TextureReference{other.m_TextureReference} {
        assert(other.m_View != nullptr && "Texture View handle cannot be nullptr");

        m_View = other.m_View;
        m_Label = other.m_Label;

        other.m_View = nullptr;
        other.m_Label = "";
        other.m_TextureReference = {};
    }

    TextureView &TextureView::operator=(TextureView &&other) {
        assert(other.m_View != nullptr && "Texture View handle cannot be nullptr");

        if (&other != this) {
            wgpuTextureViewRelease(m_View);

            m_TextureReference = other.m_TextureReference;
            m_View = other.m_View;

            other.m_View = nullptr;
        }

        return *this;
    }

    WGPUTextureView TextureView::get() const {
        assert(m_View != nullptr && "Texture View handle cannot be nullptr");

        return m_View;
    }

    const Texture &TextureView::getTextureRef() const {
        assert(m_View != nullptr && "Texture View handle cannot be nullptr");

        return m_TextureReference;
    }

    Texture &TextureView::getTextureMut() {
        assert(m_View != nullptr && "Texture View handle cannot be nullptr");

        return m_TextureReference;
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

    TextureBuilder &TextureBuilder::setUsage(TextureUsage usage) {
        m_Descriptor.usage = static_cast<WGPUTextureUsage>(usage);
        return *this;
    }

    TextureBuilder &TextureBuilder::addUsage(TextureUsage usage) {
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
        m_ViewFormats = viewFormats;
        return *this;
    }

    TextureBuilder &TextureBuilder::addViewFormat(TextureFormat viewFormat) {
        m_ViewFormats.push_back(viewFormat);
        return *this;
    }

    Texture TextureBuilder::empty3d(std::uint32_t width, std::uint32_t height, std::uint32_t depthOrArrayLayers, const GraphicsContext &context) {
        return empty(width, height, depthOrArrayLayers, context);
    }

    Texture TextureBuilder::empty2d(std::uint32_t width, std::uint32_t height, const GraphicsContext &context) {
        return empty(width, height, 1, context);
    }

    Texture TextureBuilder::build(const std::shared_ptr<TextureAsset> &asset, const GraphicsContext &context) {
        assert(asset->getData() != nullptr && asset->getSize() > 0 && "Texture data cannot be nullptr");

        Texture texture = empty(asset->getWidth(), asset->getHeight(), 1, context);

        WGPUImageCopyTexture destination{
            .texture = texture.get(),
            .mipLevel = 0,
            .origin = WGPUOrigin3D{.x = 0, .y = 0, .z = 0},
            .aspect = WGPUTextureAspect_All,
        };

        WGPUTextureDataLayout dataLayout{
            .offset = 0,
            .bytesPerRow = asset->getStride(),
            .rowsPerImage = asset->getHeight(),
        };

        wgpuQueueWriteTexture(context.getQueue(), &destination, asset->getData(), asset->getSize(), &dataLayout, &m_Descriptor.size);

        return texture;
    }

    Texture TextureBuilder::empty(std::uint32_t width, std::uint32_t height, std::uint32_t depthOrArrayLayers, const GraphicsContext &context) {
        m_Descriptor.label = m_Label.c_str();
        m_Descriptor.viewFormatCount = m_ViewFormats.size();
        m_Descriptor.viewFormats = (WGPUTextureFormat *)m_ViewFormats.data();
        m_Descriptor.size = WGPUExtent3D{.width = width, .height = height, .depthOrArrayLayers = depthOrArrayLayers};
        m_Descriptor.dimension = depthOrArrayLayers <= 1 ? WGPUTextureDimension_2D : WGPUTextureDimension_3D;

        WGPUTexture texture = wgpuDeviceCreateTexture(context.getDevice(), &m_Descriptor);

        return Texture{m_Label, texture, context};
    }

    TextureViewBuilder::TextureViewBuilder() {
        m_Label = "Yulduz Texture View";
        m_Descriptor = WGPUTextureViewDescriptor{
            .format = WGPUTextureFormat_RGBA8Unorm,
            .dimension = WGPUTextureViewDimension_2D,
            .baseMipLevel = 0,
            .mipLevelCount = 1,
            .baseArrayLayer = 0,
            .arrayLayerCount = 1,
        };
    }

    TextureViewBuilder &TextureViewBuilder::setLabel(const std::string &label) {
        m_Label = label;
        return *this;
    }

    TextureViewBuilder &TextureViewBuilder::setFormat(TextureFormat format) {
        m_Descriptor.format = static_cast<WGPUTextureFormat>(format);
        return *this;
    }

    TextureViewBuilder &TextureViewBuilder::setDimension(TextureViewDimension dimension) {
        m_Descriptor.dimension = static_cast<WGPUTextureViewDimension>(dimension);
        return *this;
    }

    TextureViewBuilder &TextureViewBuilder::setBaseMipLevel(std::uint32_t baseMipLevel) {
        m_Descriptor.baseMipLevel = baseMipLevel;
        return *this;
    }

    TextureViewBuilder &TextureViewBuilder::setBaseArrayLayer(std::uint32_t baseArrayLayer) {
        m_Descriptor.baseArrayLayer = baseArrayLayer;
        return *this;
    }

    TextureViewBuilder &TextureViewBuilder::setAspect(TextureAspect aspect) {
        m_Descriptor.aspect = static_cast<WGPUTextureAspect>(aspect);
        return *this;
    }

    TextureView TextureViewBuilder::build(const Texture &texture) {
        m_Descriptor.label = m_Label.c_str();
        m_Descriptor.mipLevelCount = texture.getMipLevelCount();
        m_Descriptor.arrayLayerCount = texture.getDepthOrArrayLayers();

        assert(m_Descriptor.baseMipLevel >= m_Descriptor.mipLevelCount && "baseMipLevel cannot be greater than mipLevelCount");
        assert(m_Descriptor.baseArrayLayer >= m_Descriptor.arrayLayerCount && "baseArrayLayer cannot be greater than arrayLayerCount");

        return TextureView{m_Label, wgpuTextureCreateView(texture.get(), &m_Descriptor), texture};
    }
}  // namespace Yulduz
