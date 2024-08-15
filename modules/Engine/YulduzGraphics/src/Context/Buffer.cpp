#include <YulduzGraphics/Context/Buffer.hpp>
#include <YulduzGraphics/Context/GraphicsContext.hpp>

namespace Yulduz {
    Buffer::Buffer(const std::string &label, const WGPUBuffer &handle, const GraphicsContext &context)
        : m_Label{label}, m_Buffer{handle}, m_ContextReference{context} {
        assert(handle != nullptr && "Buffer handle cannot be nullptr");
    }

    Buffer::Buffer() : m_ContextReference{} {
        m_Buffer = nullptr;
        m_Label = "";
    }

    Buffer::~Buffer() {
        if (m_Buffer)
            wgpuBufferRelease(m_Buffer);
    }

    Buffer::Buffer(const Buffer &other)
        : m_Label{other.m_Label}, m_Buffer{other.m_Buffer}, m_ContextReference{other.m_ContextReference} {
        assert(other.m_Buffer != nullptr && "Buffer handle cannot be nullptr");

        wgpuBufferReference(m_Buffer);
    }

    Buffer::Buffer(Buffer &&other)
        : m_Label{other.m_Label}, m_Buffer{other.m_Buffer}, m_ContextReference{std::move(other.m_ContextReference)} {
        assert(other.m_Buffer != nullptr && "Buffer handle cannot be nullptr");

        other.m_Buffer = nullptr;
        other.m_Label = "";
    }

    Buffer &Buffer::operator=(const Buffer &other) {
        assert(other.m_Buffer != nullptr && "Buffer handle cannot be nullptr");

        if (&other != nullptr) {
            if (m_Buffer) wgpuBufferRelease(m_Buffer);

            m_Buffer = other.m_Buffer;
            m_Label = other.m_Label;
            m_ContextReference = other.m_ContextReference;

            wgpuBufferReference(m_Buffer);
        }

        return *this;
    }

    Buffer &Buffer::operator=(Buffer &&other) {
        assert(other.m_Buffer != nullptr && "Buffer handle cannot be nullptr");

        if (&other != nullptr) {
            if (m_Buffer) wgpuBufferRelease(m_Buffer);

            m_Buffer = other.m_Buffer;
            m_Label = other.m_Label;
            m_ContextReference = std::move(other.m_ContextReference);

            other.m_Buffer = nullptr;
            other.m_Label = "";
        }

        return *this;
    }
    void Buffer::write(void *data) const {
        assert(m_Buffer != nullptr && "Buffer handle cannot be nullptr");

        wgpuQueueWriteBuffer(m_ContextReference.getQueue(), m_Buffer, 0, data, getSize());
    }

    void Buffer::write(void *data, std::size_t size) const {
        assert(m_Buffer != nullptr && "Buffer handle cannot be nullptr");

        wgpuQueueWriteBuffer(m_ContextReference.getQueue(), m_Buffer, 0, data, size);
    }

    void Buffer::writeWithOffset(void *data, std::size_t offset) const {
        assert(m_Buffer != nullptr && "Buffer handle cannot be nullptr");

        wgpuQueueWriteBuffer(m_ContextReference.getQueue(), m_Buffer, offset, data, getSize());
    }

    void Buffer::writeWithOffset(void *data, std::size_t size, std::size_t offset) const {
        assert(m_Buffer != nullptr && "Buffer handle cannot be nullptr");

        wgpuQueueWriteBuffer(m_ContextReference.getQueue(), m_Buffer, offset, data, size);
    }

    std::string Buffer::getLabel() const {
        assert(m_Buffer != nullptr && "Buffer handle cannot be nullptr");

        return m_Label;
    }

    WGPUBuffer Buffer::get() const {
        assert(m_Buffer != nullptr && "Buffer handle cannot be nullptr");

        return m_Buffer;
    }

    std::size_t Buffer::getSize() const {
        assert(m_Buffer != nullptr && "Buffer handle cannot be nullptr");

        return wgpuBufferGetSize(m_Buffer);
    }

    BufferUsage Buffer::getUsage() const {
        assert(m_Buffer != nullptr && "Buffer handle cannot be nullptr");

        return static_cast<BufferUsage>(wgpuBufferGetUsage(m_Buffer));
    }

    VertexBuffer::VertexBuffer(const std::string &label, const WGPUBuffer &handle, std::size_t count, std::size_t stride, const GraphicsContext &context)
        : Buffer{label, handle, context}, m_Count{count}, m_Stride{stride} {}

    VertexBuffer::VertexBuffer(const VertexBuffer &other)
        : Buffer{other}, m_Count{other.m_Count}, m_Stride{other.m_Stride} {}

    VertexBuffer::VertexBuffer(VertexBuffer &&other)
        : Buffer{std::move(other)}, m_Count{other.m_Count}, m_Stride{other.m_Stride} {
        other.m_Count = 0;
        other.m_Stride = 0;
    }

    VertexBuffer &VertexBuffer::operator=(const VertexBuffer &other) {
        if (&other != nullptr) {
            Buffer::operator=(other);
            m_Count = other.m_Count;
            m_Stride = other.m_Stride;
        }

        return *this;
    }

    VertexBuffer &VertexBuffer::operator=(VertexBuffer &&other) {
        if (&other != nullptr) {
            Buffer::operator=(std::move(other));
            m_Count = other.m_Count;
            m_Stride = other.m_Stride;
            other.m_Count = 0;
            other.m_Stride = 0;
        }

        return *this;
    }

    std::size_t VertexBuffer::getCount() const {
        assert(m_Buffer != nullptr && "Buffer handle cannot be nullptr");

        return m_Count;
    }
    std::size_t VertexBuffer::getStride() const {
        assert(m_Buffer != nullptr && "Buffer handle cannot be nullptr");

        return m_Stride;
    }

    IndexBuffer::IndexBuffer(std::string &label, const WGPUBuffer &handle, std::size_t count, IndexFormat indexFormat, const GraphicsContext &context)
        : Buffer{label, handle, context}, m_Count{count}, m_IndexFormat{indexFormat} {}

    IndexBuffer::IndexBuffer(const IndexBuffer &other)
        : Buffer{other}, m_Count{other.m_Count}, m_IndexFormat{other.m_IndexFormat} {}

    IndexBuffer::IndexBuffer(IndexBuffer &&other)
        : Buffer{std::move(other)}, m_Count{other.m_Count}, m_IndexFormat{other.m_IndexFormat} {
        other.m_Count = 0;
        other.m_IndexFormat = IndexFormat::Undefined;
    }

    IndexBuffer &IndexBuffer::operator=(const IndexBuffer &other) {
        if (&other != nullptr) {
            Buffer::operator=(other);
            m_Count = other.m_Count;
            m_IndexFormat = other.m_IndexFormat;
        }

        return *this;
    }

    IndexBuffer &IndexBuffer::operator=(IndexBuffer &&other) {
        if (&other != nullptr) {
            Buffer::operator=(std::move(other));
            m_Count = other.m_Count;
            m_IndexFormat = other.m_IndexFormat;

            other.m_Count = 0;
            other.m_IndexFormat = IndexFormat::Undefined;
        }

        return *this;
    }

    std::size_t IndexBuffer::getCount() const {
        assert(m_Buffer != nullptr && "Buffer handle cannot be nullptr");

        return m_Count;
    }

    IndexFormat IndexBuffer::getIndexFormat() const {
        assert(m_Buffer != nullptr && "Buffer handle cannot be nullptr");

        return m_IndexFormat;
    }

    UniformBuffer::UniformBuffer(const std::string &label, const WGPUBuffer &handle, const GraphicsContext &context)
        : Buffer{label, handle, context} {}

    UniformBuffer::UniformBuffer(const UniformBuffer &other)
        : Buffer{other} {}

    UniformBuffer::UniformBuffer(UniformBuffer &&other)
        : Buffer{std::move(other)} {}

    UniformBuffer &UniformBuffer::operator=(const UniformBuffer &other) {
        if (&other != nullptr) {
            Buffer::operator=(other);
        }

        return *this;
    }

    UniformBuffer &UniformBuffer::operator=(UniformBuffer &&other) {
        if (&other != nullptr) {
            Buffer::operator=(std::move(other));
        }

        return *this;
    }

    StorageBuffer::StorageBuffer(const std::string &label, const WGPUBuffer &handle, const GraphicsContext &context)
        : Buffer{label, handle, context} {}

    StorageBuffer::StorageBuffer(const StorageBuffer &other)
        : Buffer{other} {}

    StorageBuffer::StorageBuffer(StorageBuffer &&other)
        : Buffer{std::move(other)} {}

    StorageBuffer &StorageBuffer::operator=(const StorageBuffer &other) {
        if (&other != nullptr) {
            Buffer::operator=(other);
        }

        return *this;
    }

    StorageBuffer &StorageBuffer::operator=(StorageBuffer &&other) {
        if (&other != nullptr) {
            Buffer::operator=(std::move(other));
        }

        return *this;
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

    Buffer BufferBuilder::empty(std::size_t size, const GraphicsContext &context) {
        if (m_Label.empty()) m_Label = "Yulduz Buffer";

        WGPUBufferDescriptor descriptor{
            .label = m_Label.c_str(),
            .usage = WGPUBufferUsage_CopyDst | static_cast<WGPUBufferUsageFlags>(m_BufferUsage),
            .size = size,
        };

        WGPUBuffer buffer = wgpuDeviceCreateBuffer(context.getDevice(), &descriptor);

        return Buffer{m_Label, buffer, context};
    }

    Buffer BufferBuilder::build(const void *data, std::size_t size, const GraphicsContext &context) {
        if (m_Label.empty()) m_Label = "Yulduz Buffer";

        WGPUBufferDescriptor descriptor{
            .label = m_Label.c_str(),
            .usage = WGPUBufferUsage_CopyDst | static_cast<WGPUBufferUsageFlags>(m_BufferUsage),
            .size = size,
        };

        WGPUBuffer buffer = wgpuDeviceCreateBufferInit(context.getDevice(), descriptor, data);

        return Buffer{m_Label, buffer, context};
    }

    VertexBuffer BufferBuilder::emptyVertex(std::size_t size, std::size_t count, std::size_t stride, const GraphicsContext &context) {
        if (m_Label.empty()) m_Label = "Yulduz Vertex Buffer";

        WGPUBufferDescriptor descriptor{
            .label = m_Label.c_str(),
            .usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex | static_cast<WGPUBufferUsageFlags>(m_BufferUsage),
            .size = size,
        };

        WGPUBuffer buffer = wgpuDeviceCreateBuffer(context.getDevice(), &descriptor);

        return VertexBuffer{m_Label, buffer, count, stride, context};
    }

    VertexBuffer BufferBuilder::buildVertex(const void *data, std::size_t size, std::size_t count, std::size_t stride, const GraphicsContext &context) {
        if (m_Label.empty()) m_Label = "Yulduz Vertex Buffer";

        WGPUBufferDescriptor descriptor{
            .label = m_Label.c_str(),
            .usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex | static_cast<WGPUBufferUsageFlags>(m_BufferUsage),
            .size = size,
        };

        WGPUBuffer buffer = wgpuDeviceCreateBufferInit(context.getDevice(), descriptor, data);

        return VertexBuffer{m_Label, buffer, count, stride, context};
    }

    IndexBuffer BufferBuilder::emptyIndexU16(std::size_t count, const GraphicsContext &context) {
        if (m_Label.empty()) m_Label = "Yulduz Index Buffer";

        WGPUBufferDescriptor descriptor{
            .label = m_Label.c_str(),
            .usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Index | static_cast<WGPUBufferUsageFlags>(m_BufferUsage),
            .size = count * sizeof(std::uint16_t),
        };

        WGPUBuffer buffer = wgpuDeviceCreateBuffer(context.getDevice(), &descriptor);

        return IndexBuffer{m_Label, buffer, count, IndexFormat::Uint16, context};
    }

    IndexBuffer BufferBuilder::emptyIndexU32(std::size_t count, const GraphicsContext &context) {
        if (m_Label.empty()) m_Label = "Yulduz Index Buffer";

        WGPUBufferDescriptor descriptor{
            .label = m_Label.c_str(),
            .usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Index | static_cast<WGPUBufferUsageFlags>(m_BufferUsage),
            .size = count * sizeof(std::uint32_t),
        };

        WGPUBuffer buffer = wgpuDeviceCreateBuffer(context.getDevice(), &descriptor);

        return IndexBuffer{m_Label, buffer, count, IndexFormat::Uint32, context};
    }

    IndexBuffer BufferBuilder::buildIndex(const std::vector<std::uint16_t> &indices, const GraphicsContext &context) {
        if (m_Label.empty()) m_Label = "Yulduz Index Buffer";

        WGPUBufferDescriptor descriptor{
            .label = m_Label.c_str(),
            .usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Index | static_cast<WGPUBufferUsageFlags>(m_BufferUsage),
            .size = indices.size() * sizeof(std::uint16_t),
        };

        WGPUBuffer buffer = wgpuDeviceCreateBufferInit(context.getDevice(), descriptor, indices.data());

        return IndexBuffer{m_Label, buffer, indices.size(), IndexFormat::Uint16, context};
    }

    IndexBuffer BufferBuilder::buildIndex(const std::vector<std::uint32_t> &indices, const GraphicsContext &context) {
        if (m_Label.empty()) m_Label = "Yulduz Index Buffer";

        WGPUBufferDescriptor descriptor{
            .label = m_Label.c_str(),
            .usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Index | static_cast<WGPUBufferUsageFlags>(m_BufferUsage),
            .size = indices.size() * sizeof(std::uint32_t),
        };

        WGPUBuffer buffer = wgpuDeviceCreateBufferInit(context.getDevice(), descriptor, indices.data());

        return IndexBuffer{m_Label, buffer, indices.size(), IndexFormat::Uint32, context};
    }

    UniformBuffer BufferBuilder::emptyUniform(std::size_t size, const GraphicsContext &context) {
        if (m_Label.empty()) m_Label = "Yulduz Uniform Buffer";

        WGPUBufferDescriptor descriptor{
            .label = m_Label.c_str(),
            .usage = WGPUBufferUsage_Uniform | WGPUBufferUsage_CopyDst | static_cast<WGPUBufferUsageFlags>(m_BufferUsage),
            .size = size,
        };

        WGPUBuffer buffer = wgpuDeviceCreateBuffer(context.getDevice(), &descriptor);

        return UniformBuffer{m_Label, buffer, context};
    }

    UniformBuffer BufferBuilder::buildUniform(const void *data, std::size_t size, const GraphicsContext &context) {
        if (m_Label.empty()) m_Label = "Yulduz Uniform Buffer";

        WGPUBufferDescriptor descriptor{
            .label = m_Label.c_str(),
            .usage = WGPUBufferUsage_Uniform | WGPUBufferUsage_CopyDst | static_cast<WGPUBufferUsageFlags>(m_BufferUsage),
            .size = size,
        };

        WGPUBuffer buffer = wgpuDeviceCreateBufferInit(context.getDevice(), descriptor, data);

        return UniformBuffer{m_Label, buffer, context};
    }

    StorageBuffer BufferBuilder::emptyStorage(std::size_t size, const GraphicsContext &context) {
        if (m_Label.empty()) m_Label = "Yulduz Storage Buffer";

        WGPUBufferDescriptor descriptor{
            .label = m_Label.c_str(),
            .usage = WGPUBufferUsage_Storage | WGPUBufferUsage_CopyDst | static_cast<WGPUBufferUsageFlags>(m_BufferUsage),
            .size = size,
        };

        WGPUBuffer buffer = wgpuDeviceCreateBuffer(context.getDevice(), &descriptor);

        return StorageBuffer{m_Label, buffer, context};
    }

    StorageBuffer BufferBuilder::buildStorage(const void *data, std::size_t size, const GraphicsContext &context) {
        if (m_Label.empty()) m_Label = "Yulduz Storage Buffer";

        WGPUBufferDescriptor descriptor{
            .label = m_Label.c_str(),
            .usage = WGPUBufferUsage_Storage | WGPUBufferUsage_CopyDst | static_cast<WGPUBufferUsageFlags>(m_BufferUsage),
            .size = size,
        };

        WGPUBuffer buffer = wgpuDeviceCreateBufferInit(context.getDevice(), descriptor, data);

        return StorageBuffer{m_Label, buffer, context};
    }

    WGPUBuffer wgpuDeviceCreateBufferInit(WGPUDevice device, WGPUBufferDescriptor &descriptor, const void *data) {
        descriptor.mappedAtCreation = true;
        WGPUBuffer buffer = wgpuDeviceCreateBuffer(device, &descriptor);

        void *mappedData = wgpuBufferGetMappedRange(buffer, 0, descriptor.size);
        std::memcpy(mappedData, data, descriptor.size);
        wgpuBufferUnmap(buffer);

        return buffer;
    }
}  // namespace Yulduz