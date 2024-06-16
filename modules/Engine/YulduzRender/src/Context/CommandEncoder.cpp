#include <YulduzRender/Context/CommandEncoder.hpp>
#include <YulduzRender/Context.hpp>

namespace Yulduz {
    ImageCopyTexture::ImageCopyTexture(const std::shared_ptr<Texture> &texture) {
        m_Texture = texture;
        m_ImageCopyTexture = WGPUImageCopyTexture{
            .texture = texture->get(),
            .mipLevel = 0,
            .origin = WGPUOrigin3D{.x = 0, .y = 0, .z = 0},
            .aspect = WGPUTextureAspect_All,
        };
    }

    ImageCopyTexture &ImageCopyTexture::setMipLevel(uint32_t mipLevel) {
        m_ImageCopyTexture.mipLevel = mipLevel;
        return *this;
    }

    ImageCopyTexture &ImageCopyTexture::setOrigin3D(uint32_t x, uint32_t y, uint32_t z) {
        m_ImageCopyTexture.origin = WGPUOrigin3D{.x = x, .y = y, .z = z};
        return *this;
    }

    ImageCopyTexture &ImageCopyTexture::setAspect(TextureAspect aspect) {
        m_ImageCopyTexture.aspect = static_cast<WGPUTextureAspect>(aspect);
        return *this;
    }

    WGPUImageCopyTexture ImageCopyTexture::get() const {
        return m_ImageCopyTexture;
    }

    const std::shared_ptr<Texture> &ImageCopyTexture::getTexture() const {
        return m_Texture;
    }

    CommandBuffer::CommandBuffer(const std::string &label, const WGPUCommandBuffer &commandBuffer) {
        YZINFO("Initializing Command Buffer: '{}'", label);

        m_Label = label;
        m_CommandBuffer = commandBuffer;
    }

    CommandBuffer::~CommandBuffer() {
        YZINFO("Releasing Command Buffer: '{}'", m_Label);

        wgpuCommandBufferRelease(m_CommandBuffer);
    }

    std::string CommandBuffer::getLabel() const {
        return m_Label;
    }

    WGPUCommandBuffer CommandBuffer::get() const {
        return m_CommandBuffer;
    }

    CommandEncoder::CommandEncoder(const std::string &label, const WGPUCommandEncoder &commandEncoder) {
        YZINFO("Initializing Command Encoder: '{}'", label);

        m_Label = label;
        m_CommandEncoder = commandEncoder;
    }

    CommandEncoder::~CommandEncoder() {
        YZINFO("Releasing Command Encoder: '{}'", m_Label);

        wgpuCommandEncoderRelease(m_CommandEncoder);
    }

    void CommandEncoder::copyTextureToTexture(const ImageCopyTexture &src, const ImageCopyTexture &dst) {
        WGPUImageCopyTexture srcImage = src.get();
        WGPUImageCopyTexture dstImage = dst.get();
        std::array<uint32_t, 3> size = dst.getTexture()->getSize3D();
        WGPUExtent3D copySize{.width = size[0], .height = size[1], .depthOrArrayLayers = size[2]};
        wgpuCommandEncoderCopyTextureToTexture(m_CommandEncoder, &srcImage, &dstImage, &copySize);
    }

    void CommandEncoder::copyBufferToBuffer(const std::shared_ptr<Buffer> &src, const std::shared_ptr<Buffer> &dst) {
        wgpuCommandEncoderCopyBufferToBuffer(m_CommandEncoder, src->get(), 0, dst->get(), 0, dst->getSize());
    }

    void CommandEncoder::copyTextureToBuffer(const ImageCopyTexture &src, const std::shared_ptr<Buffer> &dst) {
        WGPUImageCopyTexture imageCopyTexture = src.get();
        WGPUImageCopyBuffer imageCopyBuffer{
            .layout = WGPUTextureDataLayout{
                .offset = 0,
                .bytesPerRow = src.getTexture()->getWidth() * 4,
                .rowsPerImage = src.getTexture()->getHeight(),
            },
            .buffer = dst->get(),
        };
        std::array<uint32_t, 3> size = src.getTexture()->getSize3D();
        WGPUExtent3D copySize{.width = size[0], .height = size[1], .depthOrArrayLayers = size[2]};
        wgpuCommandEncoderCopyTextureToBuffer(m_CommandEncoder, &imageCopyTexture, &imageCopyBuffer, &copySize);
    }

    void CommandEncoder::copyBufferToTexture(const std::shared_ptr<Buffer> &src, const ImageCopyTexture &dst) {
        WGPUImageCopyBuffer imageCopyBuffer{
            .layout = WGPUTextureDataLayout{
                .offset = 0,
                .bytesPerRow = dst.getTexture()->getWidth() * 4,
                .rowsPerImage = dst.getTexture()->getHeight(),
            },
            .buffer = src->get(),
        };
        WGPUImageCopyTexture imageCopyTexture = dst.get();
        std::array<uint32_t, 3> size = dst.getTexture()->getSize3D();
        WGPUExtent3D copySize{.width = size[0], .height = size[1], .depthOrArrayLayers = size[2]};
        wgpuCommandEncoderCopyBufferToTexture(m_CommandEncoder, &imageCopyBuffer, &imageCopyTexture, &copySize);
    }

    std::string CommandEncoder::getLabel() const {
        return m_Label;
    }

    WGPUCommandEncoder CommandEncoder::get() const {
        return m_CommandEncoder;
    }

    std::shared_ptr<CommandBuffer> CommandEncoder::finish(const std::string &label) {
        WGPUCommandBufferDescriptor descriptor{.label = label.c_str()};
        WGPUCommandBuffer commandBuffer = wgpuCommandEncoderFinish(m_CommandEncoder, &descriptor);
        return std::make_shared<CommandBuffer>(label, commandBuffer);
    }

    CommandEncoderBuilder::CommandEncoderBuilder() {
        m_Label = "Yulduz Command Encoder";
    }

    CommandEncoderBuilder &CommandEncoderBuilder::setLabel(const std::string &label) {
        m_Label = label;
        return *this;
    }

    std::shared_ptr<CommandEncoder> CommandEncoderBuilder::build(const std::shared_ptr<RenderContext> &context) {
        WGPUCommandEncoderDescriptor descriptor{.label = m_Label.c_str()};
        WGPUCommandEncoder commandEncoder = wgpuDeviceCreateCommandEncoder(context->getDevice(), &descriptor);
        return std::make_shared<CommandEncoder>(m_Label, commandEncoder);
    }
}  // namespace Yulduz
