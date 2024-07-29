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

    void Buffer::write(void *data, std::size_t size, const std::shared_ptr<RenderContext> &context) const {
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

    void Buffer::writeWithOffset(void *data, std::size_t offset, const std::shared_ptr<RenderContext> &context) const {
        if (data == nullptr) {
            YZERROR("Buffer: '{}' data is nullptr", m_Label);
            return;
        }

        wgpuQueueWriteBuffer(context->getQueue(), m_Buffer, offset, data, wgpuBufferGetSize(m_Buffer));
    }

    void Buffer::writeWithOffset(void *data, std::size_t offset, std::size_t size, const std::shared_ptr<RenderContext> &context) const {
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

    std::size_t Buffer::getSize() const {
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

    BufferBuilder &BufferBuilder::setBufferUsageReq(BufferUsage usage) {
        m_BufferUsage = usage;
        return *this;
    }

    BufferBuilder &BufferBuilder::addBufferUsage(BufferUsage usage) {
        m_BufferUsage |= usage;
        return *this;
    }

    std::shared_ptr<Buffer> BufferBuilder::empty(std::size_t size, const std::shared_ptr<RenderContext> &context) {
        WGPUBufferDescriptor descriptor{
            .label = m_Label.c_str(),
            .usage = static_cast<WGPUBufferUsageFlags>(m_BufferUsage),
            .size = size,
        };

        WGPUBuffer buffer = wgpuDeviceCreateBuffer(context->getDevice(), &descriptor);

        return std::make_shared<Buffer>(m_Label, buffer);
    }

    std::shared_ptr<Buffer> BufferBuilder::build(const void *data, std::size_t size, const std::shared_ptr<RenderContext> &context) {
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

    VertexBuffer::VertexBuffer(const std::string &label, const WGPUBuffer &buffer, std::size_t count, std::size_t stride) {
        YZDEBUG("Initializing Vertex Buffer: '{}'", label);

        m_Label = label;
        m_Buffer = buffer;
        m_Count = count;
        m_Stride = stride;
        m_IsDefaultBuffer = false;
    }

    VertexBuffer::~VertexBuffer() {
        YZDEBUG("Releasing Vertex Buffer: '{}'", m_Label);

        wgpuBufferRelease(m_Buffer);
    }

    std::size_t VertexBuffer::getCount() const {
        return m_Count;
    }

    std::size_t VertexBuffer::getStride() const {
        return m_Stride;
    }

    IndexBuffer::IndexBuffer(const std::string &label, const WGPUBuffer &buffer, std::size_t count, IndexFormat indexFormat) {
        YZDEBUG("Initializing Index Buffer: '{}'", label);

        m_Label = label;
        m_Buffer = buffer;
        m_Count = count;
        m_IndexFormat = indexFormat;
        m_IsDefaultBuffer = false;
    }

    IndexBuffer::~IndexBuffer() {
        YZDEBUG("Releasing Index Buffer: '{}'", m_Label);

        wgpuBufferRelease(m_Buffer);
    }

    std::size_t IndexBuffer::getCount() const {
        return m_Count;
    }

    IndexFormat IndexBuffer::getIndexFormat() const {
        return m_IndexFormat;
    }

    UniformBuffer::UniformBuffer(const std::string &label, const WGPUBuffer &buffer) {
        YZDEBUG("Initializing Uniform Buffer: '{}'", label);

        m_Label = label;
        m_Buffer = buffer;
        m_IsDefaultBuffer = false;
    }

    UniformBuffer::~UniformBuffer() {
        YZDEBUG("Releasing Uniform Buffer: '{}'", m_Label);

        wgpuBufferRelease(m_Buffer);
    }

    StorageBuffer::StorageBuffer(const std::string &label, const WGPUBuffer &buffer) {
        YZDEBUG("Initializing Storage Buffer: '{}'", label);

        m_Label = label;
        m_Buffer = buffer;
        m_IsDefaultBuffer = false;
    }

    StorageBuffer::~StorageBuffer() {
        YZDEBUG("Releasing Storage Buffer: '{}'", m_Label);

        wgpuBufferRelease(m_Buffer);
    }

    VertexBufferBuilder::VertexBufferBuilder() {
        m_Label = "Yulduz Vertex Buffer";
        m_BufferUsage = BufferUsage::None;
    }

    VertexBufferBuilder &VertexBufferBuilder::setLabel(const std::string &label) {
        m_Label = label;
        return *this;
    }

    VertexBufferBuilder &VertexBufferBuilder::setBufferUsage(BufferUsage usage) {
        m_BufferUsage = usage;
        return *this;
    }

    VertexBufferBuilder &VertexBufferBuilder::addBufferUsage(BufferUsage usage) {
        m_BufferUsage |= usage;
        return *this;
    }

    std::shared_ptr<VertexBuffer> VertexBufferBuilder::empty(std::size_t size, std::size_t count, std::size_t stride, const std::shared_ptr<RenderContext> &context) {
        WGPUBufferDescriptor descriptor{
            .label = m_Label.c_str(),
            .usage = static_cast<WGPUBufferUsageFlags>(m_BufferUsage) | WGPUBufferUsage_Vertex | WGPUBufferUsage_CopyDst,
            .size = size,
        };

        WGPUBuffer buffer = wgpuDeviceCreateBuffer(context->getDevice(), &descriptor);

        return std::make_shared<VertexBuffer>(m_Label, buffer, count, stride);
    }

    std::shared_ptr<VertexBuffer> VertexBufferBuilder::build(const void *data, std::size_t size, std::size_t count, std::size_t stride, const std::shared_ptr<RenderContext> &context) {
        WGPUBufferDescriptor descriptor{
            .label = m_Label.c_str(),
            .usage = static_cast<WGPUBufferUsageFlags>(m_BufferUsage) | WGPUBufferUsage_Vertex | WGPUBufferUsage_CopyDst,
            .size = size,
        };

        WGPUBuffer buffer = wgpuDeviceCreateBufferInit(context->getDevice(), descriptor, data);

        return std::make_shared<VertexBuffer>(m_Label, buffer, count, stride);
    }

    IndexBufferBuilder::IndexBufferBuilder() {
        m_Label = "Yulduz Index Buffer";
        m_BufferUsage = BufferUsage::None;
    }

    IndexBufferBuilder &IndexBufferBuilder::setLabel(const std::string &label) {
        m_Label = label;
        return *this;
    }

    IndexBufferBuilder &IndexBufferBuilder::setBufferUsage(BufferUsage usage) {
        m_BufferUsage = usage;
        return *this;
    }

    IndexBufferBuilder &IndexBufferBuilder::addBufferUsage(BufferUsage usage) {
        m_BufferUsage |= usage;
        return *this;
    }

    std::shared_ptr<IndexBuffer> IndexBufferBuilder::emptyU16(std::size_t count, const std::shared_ptr<RenderContext> &context) {
        WGPUBufferDescriptor descriptor{
            .label = m_Label.c_str(),
            .usage = static_cast<WGPUBufferUsageFlags>(m_BufferUsage) | WGPUBufferUsage_Index | WGPUBufferUsage_CopyDst,
            .size = sizeof(uint16_t) * count,
        };

        WGPUBuffer buffer = wgpuDeviceCreateBuffer(context->getDevice(), &descriptor);

        return std::make_shared<IndexBuffer>(m_Label, buffer, count, IndexFormat::Uint16);
    }

    std::shared_ptr<IndexBuffer> IndexBufferBuilder::emptyU32(std::size_t count, const std::shared_ptr<RenderContext> &context) {
        WGPUBufferDescriptor descriptor{
            .label = m_Label.c_str(),
            .usage = static_cast<WGPUBufferUsageFlags>(m_BufferUsage) | WGPUBufferUsage_Index | WGPUBufferUsage_CopyDst,
            .size = sizeof(uint32_t) * count,
        };

        WGPUBuffer buffer = wgpuDeviceCreateBuffer(context->getDevice(), &descriptor);

        return std::make_shared<IndexBuffer>(m_Label, buffer, count, IndexFormat::Uint32);
    }

    std::shared_ptr<IndexBuffer> IndexBufferBuilder::build(const std::vector<uint16_t> &indices, const std::shared_ptr<RenderContext> &context) {
        WGPUBufferDescriptor descriptor{
            .label = m_Label.c_str(),
            .usage = static_cast<WGPUBufferUsageFlags>(m_BufferUsage) | WGPUBufferUsage_Index | WGPUBufferUsage_CopyDst,
            .size = sizeof(uint16_t) * indices.size(),
        };

        WGPUBuffer buffer = wgpuDeviceCreateBufferInit(context->getDevice(), descriptor, indices.data());

        return std::make_shared<IndexBuffer>(m_Label, buffer, indices.size(), IndexFormat::Uint16);
    }

    std::shared_ptr<IndexBuffer> IndexBufferBuilder::build(const std::vector<uint32_t> &indices, const std::shared_ptr<RenderContext> &context) {
        WGPUBufferDescriptor descriptor{
            .label = m_Label.c_str(),
            .usage = static_cast<WGPUBufferUsageFlags>(m_BufferUsage) | WGPUBufferUsage_Index | WGPUBufferUsage_CopyDst,
            .size = sizeof(uint32_t) * indices.size(),
        };

        WGPUBuffer buffer = wgpuDeviceCreateBufferInit(context->getDevice(), descriptor, indices.data());

        return std::make_shared<IndexBuffer>(m_Label, buffer, indices.size(), IndexFormat::Uint32);
    }

    StorageBufferBuilder::StorageBufferBuilder() {
        m_Label = "Yulduz Storage Buffer";
        m_BufferUsage = BufferUsage::None;
    }

    StorageBufferBuilder &StorageBufferBuilder::setLabel(const std::string &label) {
        m_Label = label;
        return *this;
    }

    StorageBufferBuilder &StorageBufferBuilder::setBufferUsage(BufferUsage usage) {
        m_BufferUsage = usage;
        return *this;
    }

    StorageBufferBuilder &StorageBufferBuilder::addBufferUsage(BufferUsage usage) {
        m_BufferUsage |= usage;
        return *this;
    }

    std::shared_ptr<StorageBuffer> StorageBufferBuilder::empty(std::size_t count, const std::shared_ptr<RenderContext> &context) {
        WGPUBufferDescriptor descriptor{
            .label = m_Label.c_str(),
            .usage = static_cast<WGPUBufferUsageFlags>(m_BufferUsage) | WGPUBufferUsage_CopyDst | WGPUBufferUsage_Storage,
            .size = sizeof(float) * count,
        };

        WGPUBuffer buffer = wgpuDeviceCreateBuffer(context->getDevice(), &descriptor);

        return std::make_shared<StorageBuffer>(m_Label, buffer);
    }

    std::shared_ptr<StorageBuffer> StorageBufferBuilder::build(const void *data, std::size_t size, const std::shared_ptr<RenderContext> &context) {
        WGPUBufferDescriptor descriptor{
            .label = m_Label.c_str(),
            .usage = static_cast<WGPUBufferUsageFlags>(m_BufferUsage) | WGPUBufferUsage_CopyDst | WGPUBufferUsage_Storage,
            .size = size,
        };

        WGPUBuffer buffer = wgpuDeviceCreateBufferInit(context->getDevice(), descriptor, data);

        return std::make_shared<StorageBuffer>(m_Label, buffer);
    }

    UniformBufferBuilder::UniformBufferBuilder() {
        m_Label = "Yulduz Uniform Buffer";
        m_BufferUsage = BufferUsage::None;
    }

    UniformBufferBuilder &UniformBufferBuilder::setLabel(const std::string &label) {
        m_Label = label;
        return *this;
    }

    UniformBufferBuilder &UniformBufferBuilder::setBufferUsage(BufferUsage usage) {
        m_BufferUsage = usage;
        return *this;
    }

    UniformBufferBuilder &UniformBufferBuilder::addBufferUsage(BufferUsage usage) {
        m_BufferUsage |= usage;
        return *this;
    }

    std::shared_ptr<UniformBuffer> UniformBufferBuilder::empty(std::size_t count, const std::shared_ptr<RenderContext> &context) {
        WGPUBufferDescriptor descriptor{
            .label = m_Label.c_str(),
            .usage = static_cast<WGPUBufferUsageFlags>(m_BufferUsage) | WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform,
            .size = sizeof(float) * count,
        };

        WGPUBuffer buffer = wgpuDeviceCreateBuffer(context->getDevice(), &descriptor);

        return std::make_shared<UniformBuffer>(m_Label, buffer);
    }

    std::shared_ptr<UniformBuffer> UniformBufferBuilder::build(const void *data, std::size_t size, const std::shared_ptr<RenderContext> &context) {
        WGPUBufferDescriptor descriptor{
            .label = m_Label.c_str(),
            .usage = static_cast<WGPUBufferUsageFlags>(m_BufferUsage) | WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform,
            .size = size,
        };

        WGPUBuffer buffer = wgpuDeviceCreateBufferInit(context->getDevice(), descriptor, data);

        return std::make_shared<UniformBuffer>(m_Label, buffer);
    }
}  // namespace Yulduz
