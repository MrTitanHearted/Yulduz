#include <YulduzRender/Context/Buffer.hpp>
#include <YulduzRender/Context.hpp>

namespace Yulduz {
    Buffer::Buffer(const std::string &label, const WGPUBuffer &buffer) {
        YZDEBUG("Initializing Buffer: '{}'", label);

        m_Label = label;
        m_Buffer = buffer;
        m_IsDefaultBuffer = true;
    }

    Buffer::~Buffer() {
        if (!m_IsDefaultBuffer) return;
        YZDEBUG("Releasing Buffer: '{}'", m_Label);

        wgpuBufferRelease(m_Buffer);
    }

    void Buffer::write(void *data, const std::shared_ptr<RenderContext> &context) const {
        if (data == nullptr) {
            YZERROR("Buffer: '{}' data is nullptr", m_Label);
            return;
        }

        wgpuQueueWriteBuffer(context->getQueue(), m_Buffer, 0, data, wgpuBufferGetSize(m_Buffer));
    }

    void Buffer::write(void *data, size_t size, const std::shared_ptr<RenderContext> &context) const {
        if (data == nullptr) {
            YZERROR("Buffer: '{}' data is nullptr", m_Label);
            return;
        }

        if (wgpuBufferGetSize(m_Buffer) < size || size <= 0) {
            YZERROR("Buffer: '{}' wrong data size: expected {}, got {}", m_Label, wgpuBufferGetSize(m_Buffer), size);
            return;
        }

        wgpuQueueWriteBuffer(context->getQueue(), m_Buffer, 0, data, size);
    }

    void Buffer::writeWithOffset(void *data, size_t offset, const std::shared_ptr<RenderContext> &context) const {
        if (data == nullptr) {
            YZERROR("Buffer: '{}' data is nullptr", m_Label);
            return;
        }

        wgpuQueueWriteBuffer(context->getQueue(), m_Buffer, offset, data, wgpuBufferGetSize(m_Buffer));
    }

    void Buffer::writeWithOffset(void *data, size_t offset, size_t size, const std::shared_ptr<RenderContext> &context) const {
        if (data == nullptr) {
            YZERROR("Buffer: '{}' data is nullptr", m_Label);
            return;
        }

        if (wgpuBufferGetSize(m_Buffer) < size || size <= 0) {
            YZERROR("Buffer: '{}' wrong data size: expected {}, got {}", m_Label, wgpuBufferGetSize(m_Buffer), size);
            return;
        }

        wgpuQueueWriteBuffer(context->getQueue(), m_Buffer, offset, data, size);
    }

    std::string Buffer::getLabel() const {
        return m_Label;
    }

    WGPUBuffer Buffer::get() const {
        return m_Buffer;
    }

    size_t Buffer::getSize() const {
        return wgpuBufferGetSize(m_Buffer);
    }

    BufferUsage Buffer::getUsage() const {
        return static_cast<BufferUsage>(wgpuBufferGetUsage(m_Buffer));
    }

    BufferBuilder::BufferBuilder() {
        m_Label = "Yulduz Buffer";
        m_BufferUsage = BufferUsage::None;
    }

    BufferBuilder &BufferBuilder::setLabel(const std::string &label) {
        m_Label = label;
        return *this;
    }

    BufferBuilder &BufferBuilder::setBufferUsage(BufferUsage usage) {
        m_BufferUsage = usage;
        return *this;
    }

    BufferBuilder &BufferBuilder::addBufferUsage(BufferUsage usage) {
        m_BufferUsage |= usage;
        return *this;
    }

    std::shared_ptr<Buffer> BufferBuilder::empty(size_t size, const std::shared_ptr<RenderContext> &context) {
        WGPUBufferDescriptor descriptor{
            .label = m_Label.c_str(),
            .usage = static_cast<WGPUBufferUsageFlags>(m_BufferUsage),
            .size = size,
        };

        WGPUBuffer buffer = wgpuDeviceCreateBuffer(context->getDevice(), &descriptor);

        return std::make_shared<Buffer>(m_Label, buffer);
    }

    std::shared_ptr<Buffer> BufferBuilder::build(const void *data, size_t size, const std::shared_ptr<RenderContext> &context) {
        if (data == nullptr) {
            YZERROR("Buffer: '{}' data is nullptr", m_Label);
            return nullptr;
        }

        WGPUBufferDescriptor descriptor{
            .label = m_Label.c_str(),
            .usage = static_cast<WGPUBufferUsageFlags>(m_BufferUsage),
            .size = size,
        };

        WGPUBuffer buffer = wgpuDeviceCreateBufferInit(context->getDevice(), descriptor, data);

        return std::make_shared<Buffer>(m_Label, buffer);
    }

    WGPUBuffer wgpuDeviceCreateBufferInit(WGPUDevice device, WGPUBufferDescriptor descriptor, const void *data) {
        descriptor.mappedAtCreation = true;
        WGPUBuffer buffer = wgpuDeviceCreateBuffer(device, &descriptor);

        void *mappedData = wgpuBufferGetMappedRange(buffer, 0, descriptor.size);
        memcpy(mappedData, data, descriptor.size);
        wgpuBufferUnmap(buffer);

        return buffer;
    }
}  // namespace Yulduz
