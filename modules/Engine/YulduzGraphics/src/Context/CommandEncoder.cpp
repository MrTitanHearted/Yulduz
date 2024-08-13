#include <YulduzGraphics/Context/CommandEncoder.hpp>
#include <YulduzGraphics/Context/GraphicsContext.hpp>

namespace Yulduz {
    CommandBuffer::CommandBuffer(const WGPUCommandBuffer &handle) {
        assert(handle != nullptr && "Command Buffer handle cannot be nullptr");

        m_CommandBuffer = handle;
    }

    CommandBuffer::~CommandBuffer() {
        if (m_CommandBuffer) {
            wgpuCommandBufferRelease(m_CommandBuffer);
        }
    }

    CommandBuffer::CommandBuffer(const CommandBuffer &other) {
        assert(other.m_CommandBuffer != nullptr && "Command Buffer handle cannot be nullptr");

        wgpuCommandBufferReference(other.m_CommandBuffer);

        m_CommandBuffer = other.m_CommandBuffer;
    }

    CommandBuffer &CommandBuffer::operator=(const CommandBuffer &other) {
        assert(other.m_CommandBuffer != nullptr && "Command Buffer handle cannot be nullptr");

        if (&other != this) {
            wgpuCommandBufferRelease(m_CommandBuffer);

            m_CommandBuffer = other.m_CommandBuffer;

            wgpuCommandBufferReference(m_CommandBuffer);
        }

        return *this;
    }

    CommandBuffer::CommandBuffer(CommandBuffer &&other) {
        assert(other.m_CommandBuffer != nullptr && "Command Buffer handle cannot be nullptr");

        m_CommandBuffer = other.m_CommandBuffer;

        other.m_CommandBuffer = nullptr;
    }

    CommandBuffer &CommandBuffer::operator=(CommandBuffer &&other) {
        assert(other.m_CommandBuffer != nullptr && "Command Buffer handle cannot be nullptr");

        if (&other != this) {
            wgpuCommandBufferRelease(m_CommandBuffer);

            m_CommandBuffer = other.m_CommandBuffer;

            other.m_CommandBuffer = nullptr;
        }

        return *this;
    }

    WGPUCommandBuffer CommandBuffer::get() const {
        assert(m_CommandBuffer != nullptr && "Command Buffer handle cannot be nullptr");

        return m_CommandBuffer;
    }

    CommandEncoder::CommandEncoder(const std::string &label, const WGPUCommandEncoder &handle) {
        assert(handle != nullptr && "Command Encoder handle cannot be nullptr");

        m_CommandEncoder = handle;
        m_Label = label;
    }

    CommandEncoder::~CommandEncoder() {
        if (m_CommandEncoder) {
            wgpuCommandEncoderRelease(m_CommandEncoder);
        }
    }

    CommandEncoder::CommandEncoder(const CommandEncoder &other) {
        assert(other.m_CommandEncoder != nullptr && "Command Encoder handle cannot be nullptr");

        wgpuCommandEncoderReference(other.m_CommandEncoder);

        m_CommandEncoder = other.m_CommandEncoder;
        m_Label = other.m_Label;
    }

    CommandEncoder &CommandEncoder::operator=(const CommandEncoder &other) {
        assert(other.m_CommandEncoder != nullptr && "Command Encoder handle cannot be nullptr");

        if (&other != this) {
            wgpuCommandEncoderRelease(m_CommandEncoder);

            m_CommandEncoder = other.m_CommandEncoder;
            m_Label = other.m_Label;

            wgpuCommandEncoderReference(m_CommandEncoder);
        }

        return *this;
    }

    CommandEncoder::CommandEncoder(CommandEncoder &&other) {
        assert(other.m_CommandEncoder != nullptr && "Command Encoder handle cannot be nullptr");

        m_CommandEncoder = other.m_CommandEncoder;
        m_Label = other.m_Label;

        other.m_CommandEncoder = nullptr;
        other.m_Label = "";
    }

    CommandEncoder &CommandEncoder::operator=(CommandEncoder &&other) {
        assert(other.m_CommandEncoder != nullptr && "Command Encoder handle cannot be nullptr");

        if (&other != this) {
            wgpuCommandEncoderRelease(m_CommandEncoder);

            m_CommandEncoder = other.m_CommandEncoder;
            m_Label = other.m_Label;

            other.m_CommandEncoder = nullptr;
            other.m_Label = "";
        }

        return *this;
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
