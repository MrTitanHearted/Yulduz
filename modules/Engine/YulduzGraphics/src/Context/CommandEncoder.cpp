#include <YulduzGraphics/Context/CommandEncoder.hpp>
#include <YulduzGraphics/Context/GraphicsContext.hpp>
#include <YulduzGraphics/Context/Buffer.hpp>
#include <YulduzGraphics/Context/Texture.hpp>

namespace Yulduz {
    ImageCopyTexture::ImageCopyTexture(const Texture &texture) : m_Texture{texture} {
        m_ImageCopyTexture = WGPUImageCopyTexture{
            .texture = texture.get(),
            .mipLevel = 0,
            .origin = WGPUOrigin3D{.x = 0, .y = 0, .z = 0},
            .aspect = WGPUTextureAspect_All,
        };
    }

    void ImageCopyTexture::write(const void *data) const {
        WGPUTextureDataLayout dataLayout{
            .offset = 0,
            .bytesPerRow = m_Texture.getWidth() * m_Texture.getFormatSize(),
            .rowsPerImage = m_Texture.getHeight() * m_Texture.getDepthOrArrayLayers(),
        };
        WGPUExtent3D writeSize{
            .width = m_Texture.getWidth(),
            .height = m_Texture.getHeight(),
            .depthOrArrayLayers = m_Texture.getDepthOrArrayLayers(),
        };

        wgpuQueueWriteTexture(m_Texture.getContextRef().getQueue(),
                              &m_ImageCopyTexture,
                              data,
                              m_Texture.getWidth() * m_Texture.getHeight() * m_Texture.getDepthOrArrayLayers() * m_Texture.getFormatSize(),
                              &dataLayout,
                              &writeSize);
    }

    ImageCopyTexture &ImageCopyTexture::setMipLevel(std::uint32_t mipLevel) {
        m_ImageCopyTexture.mipLevel = mipLevel;
        return *this;
    }

    ImageCopyTexture &ImageCopyTexture::setOrigin3D(std::uint32_t x, std::uint32_t y, std::uint32_t z) {
        m_ImageCopyTexture.origin = WGPUOrigin3D{.x = x, .y = y, .z = z};
        return *this;
    }

    ImageCopyTexture &ImageCopyTexture::setAspect(TextureAspect aspect) {
        m_ImageCopyTexture.aspect = static_cast<WGPUTextureAspect>(aspect);
        return *this;
    }

    const WGPUImageCopyTexture &ImageCopyTexture::getRef() const {
        return m_ImageCopyTexture;
    }

    const Texture &ImageCopyTexture::getTextureRef() const {
        return m_Texture;
    }

    CommandBuffer::CommandBuffer(const WGPUCommandBuffer &handle) : m_CommandBuffer{handle} {
        assert(handle != nullptr && "Command Buffer handle cannot be nullptr");
    }

    CommandBuffer::CommandBuffer() : m_CommandBuffer{nullptr} {}

    CommandBuffer::~CommandBuffer() {
        if (m_CommandBuffer) {
            wgpuCommandBufferRelease(m_CommandBuffer);
        }
    }

    CommandBuffer::CommandBuffer(const CommandBuffer &other)
        : m_CommandBuffer{other.m_CommandBuffer} {
        assert(other.m_CommandBuffer != nullptr && "Command Buffer handle cannot be nullptr");
        wgpuCommandBufferReference(m_CommandBuffer);
    }

    CommandBuffer::CommandBuffer(CommandBuffer &&other)
        : m_CommandBuffer{other.m_CommandBuffer} {
        assert(other.m_CommandBuffer != nullptr && "Command Buffer handle cannot be nullptr");
        other.m_CommandBuffer = nullptr;
    }

    CommandBuffer &CommandBuffer::operator=(const CommandBuffer &other) {
        assert(other.m_CommandBuffer != nullptr && "Command Buffer handle cannot be nullptr");

        if (&other != this) {
            if (m_CommandBuffer) wgpuCommandBufferRelease(m_CommandBuffer);

            m_CommandBuffer = other.m_CommandBuffer;

            wgpuCommandBufferReference(m_CommandBuffer);
        }

        return *this;
    }

    CommandBuffer &CommandBuffer::operator=(CommandBuffer &&other) {
        assert(other.m_CommandBuffer != nullptr && "Command Buffer handle cannot be nullptr");
        if (&other != this) {
            if (m_CommandBuffer) wgpuCommandBufferRelease(m_CommandBuffer);

            m_CommandBuffer = other.m_CommandBuffer;

            other.m_CommandBuffer = nullptr;
        }

        return *this;
    }

    WGPUCommandBuffer CommandBuffer::get() const {
        assert(m_CommandBuffer != nullptr && "Command Buffer handle cannot be nullptr");

        return m_CommandBuffer;
    }

    CommandEncoder::CommandEncoder(const std::string &label, const WGPUCommandEncoder &handle)
        : m_Label{label}, m_CommandEncoder{handle} {
        assert(handle != nullptr && "Command Encoder handle cannot be nullptr");
    }

    CommandEncoder::CommandEncoder()
        : m_Label{""}, m_CommandEncoder{nullptr} {}

    CommandEncoder::~CommandEncoder() {
        if (m_CommandEncoder) {
            wgpuCommandEncoderRelease(m_CommandEncoder);
        }
    }

    CommandEncoder::CommandEncoder(const CommandEncoder &other)
        : m_Label{other.m_Label}, m_CommandEncoder{other.m_CommandEncoder} {
        assert(other.m_CommandEncoder != nullptr && "Command Encoder handle cannot be nullptr");
        wgpuCommandEncoderReference(m_CommandEncoder);
    }

    CommandEncoder::CommandEncoder(CommandEncoder &&other)
        : m_Label{other.m_Label}, m_CommandEncoder{other.m_CommandEncoder} {
        assert(other.m_CommandEncoder != nullptr && "Command Encoder handle cannot be nullptr");
        other.m_CommandEncoder = nullptr;
        other.m_Label = "";
    }

    CommandEncoder &CommandEncoder::operator=(const CommandEncoder &other) {
        assert(other.m_CommandEncoder != nullptr && "Command Encoder handle cannot be nullptr");

        if (&other != this) {
            if (m_CommandEncoder) wgpuCommandEncoderRelease(m_CommandEncoder);

            m_CommandEncoder = other.m_CommandEncoder;
            m_Label = other.m_Label;

            wgpuCommandEncoderReference(m_CommandEncoder);
        }

        return *this;
    }

    CommandEncoder &CommandEncoder::operator=(CommandEncoder &&other) {
        assert(other.m_CommandEncoder != nullptr && "Command Encoder handle cannot be nullptr");

        if (&other != this) {
            if (m_CommandEncoder) wgpuCommandEncoderRelease(m_CommandEncoder);

            m_CommandEncoder = other.m_CommandEncoder;
            m_Label = other.m_Label;

            other.m_CommandEncoder = nullptr;
            other.m_Label = "";
        }

        return *this;
    }

    void CommandEncoder::copyTextureToTexture(const ImageCopyTexture &src, const ImageCopyTexture &dst) {
        auto [width, height, depthOrArrayLayers] = dst.getTextureRef().getSize3D();
        WGPUExtent3D copySize{.width = width, .height = height, .depthOrArrayLayers = depthOrArrayLayers};
        wgpuCommandEncoderCopyTextureToTexture(m_CommandEncoder, &src.getRef(), &dst.getRef(), &copySize);
    }

    void CommandEncoder::copyBufferToBuffer(const Buffer &src, const Buffer &dst) {
        wgpuCommandEncoderCopyBufferToBuffer(m_CommandEncoder, src.get(), 0, dst.get(), 0, dst.getSize());
    }

    void CommandEncoder::copyTextureToBuffer(const ImageCopyTexture &src, const Buffer &dst) {
        const Texture &srcTexture = src.getTextureRef();
        auto [width, height, depthOrArrayLayers] = srcTexture.getSize3D();
        WGPUExtent3D copySize{.width = width, .height = height, .depthOrArrayLayers = depthOrArrayLayers};
        WGPUImageCopyBuffer imageCopyBuffer{
            .layout = WGPUTextureDataLayout{
                .offset = 0,
                .bytesPerRow = width * srcTexture.getFormatSize(),
                .rowsPerImage = height * depthOrArrayLayers,
            },
            .buffer = dst.get(),
        };
        wgpuCommandEncoderCopyTextureToBuffer(m_CommandEncoder, &src.getRef(), &imageCopyBuffer, &copySize);
    }

    void CommandEncoder::copyBufferToTexture(const Buffer &src, const ImageCopyTexture &dst) {
        const Texture &dstTexture = dst.getTextureRef();
        auto [width, height, depthOrArrayLayers] = dstTexture.getSize3D();
        WGPUExtent3D copySize{.width = width, .height = height, .depthOrArrayLayers = depthOrArrayLayers};
        WGPUImageCopyBuffer imageCopyBuffer{
            .layout = WGPUTextureDataLayout{
                .offset = 0,
                .bytesPerRow = width * dstTexture.getFormatSize(),
                .rowsPerImage = height * depthOrArrayLayers,
            },
            .buffer = src.get(),
        };
        wgpuCommandEncoderCopyBufferToTexture(m_CommandEncoder, &imageCopyBuffer, &dst.getRef(), &copySize);
    }

    WGPUCommandEncoder CommandEncoder::get() const {
        assert(m_CommandEncoder != nullptr && "Command Encoder handle cannot be nullptr");

        return m_CommandEncoder;
    }

    std::string CommandEncoder::getLabel() const {
        return m_Label;
    }

    CommandBuffer CommandEncoder::finish(const std::string &label) {
        assert(m_CommandEncoder != nullptr && "Command Encoder handle cannot be nullptr");

        WGPUCommandBufferDescriptor descriptor{.label = label.c_str()};
        return CommandBuffer{wgpuCommandEncoderFinish(m_CommandEncoder, &descriptor)};
    }

    CommandEncoderBuilder::CommandEncoderBuilder() {
        m_Label = "Yulduz Command Encoder";
    }

    CommandEncoderBuilder &CommandEncoderBuilder::setLabel(const std::string &label) {
        m_Label = label;
        return *this;
    }

    CommandEncoder CommandEncoderBuilder::build(const GraphicsContext &context) {
        WGPUCommandEncoderDescriptor descriptor{.label = m_Label.c_str()};
        return CommandEncoder{m_Label, wgpuDeviceCreateCommandEncoder(context.getDevice(), &descriptor)};
    }
}  // namespace Yulduz
