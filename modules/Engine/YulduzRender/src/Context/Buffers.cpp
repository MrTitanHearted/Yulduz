#include <YulduzRender/Context/Buffers.hpp>
#include <YulduzRender/Context.hpp>

namespace Yulduz {
    VertexBuffer::VertexBuffer(const std::string &label, const WGPUBuffer &buffer, size_t count, size_t stride) {
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

    size_t VertexBuffer::getCount() const {
        return m_Count;
    }

    size_t VertexBuffer::getStride() const {
        return m_Stride;
    }

    IndexBuffer::IndexBuffer(const std::string &label, const WGPUBuffer &buffer, size_t count, IndexFormat indexFormat) {
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

    size_t IndexBuffer::getCount() const {
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

    std::shared_ptr<VertexBuffer> VertexBufferBuilder::empty(size_t size, size_t count, size_t stride, const std::shared_ptr<RenderContext> &context) {
        WGPUBufferDescriptor descriptor{
            .label = m_Label.c_str(),
            .usage = static_cast<WGPUBufferUsageFlags>(m_BufferUsage) | WGPUBufferUsage_Vertex | WGPUBufferUsage_CopyDst,
            .size = size,
        };

        WGPUBuffer buffer = wgpuDeviceCreateBuffer(context->getDevice(), &descriptor);

        return std::make_shared<VertexBuffer>(m_Label, buffer, count, stride);
    }

    std::shared_ptr<VertexBuffer> VertexBufferBuilder::build(const void *data, size_t size, size_t count, size_t stride, const std::shared_ptr<RenderContext> &context) {
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

    std::shared_ptr<IndexBuffer> IndexBufferBuilder::emptyU16(size_t count, const std::shared_ptr<RenderContext> &context) {
        WGPUBufferDescriptor descriptor{
            .label = m_Label.c_str(),
            .usage = static_cast<WGPUBufferUsageFlags>(m_BufferUsage) | WGPUBufferUsage_Index | WGPUBufferUsage_CopyDst,
            .size = sizeof(uint16_t) * count,
        };

        WGPUBuffer buffer = wgpuDeviceCreateBuffer(context->getDevice(), &descriptor);

        return std::make_shared<IndexBuffer>(m_Label, buffer, count, IndexFormat::Uint16);
    }

    std::shared_ptr<IndexBuffer> IndexBufferBuilder::emptyU32(size_t count, const std::shared_ptr<RenderContext> &context) {
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

    std::shared_ptr<StorageBuffer> StorageBufferBuilder::empty(size_t count, const std::shared_ptr<RenderContext> &context) {
        WGPUBufferDescriptor descriptor{
            .label = m_Label.c_str(),
            .usage = static_cast<WGPUBufferUsageFlags>(m_BufferUsage) | WGPUBufferUsage_CopyDst | WGPUBufferUsage_Storage,
            .size = sizeof(float) * count,
        };

        WGPUBuffer buffer = wgpuDeviceCreateBuffer(context->getDevice(), &descriptor);

        return std::make_shared<StorageBuffer>(m_Label, buffer);
    }

    std::shared_ptr<StorageBuffer> StorageBufferBuilder::build(const void *data, size_t size, const std::shared_ptr<RenderContext> &context) {
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

    std::shared_ptr<UniformBuffer> UniformBufferBuilder::empty(size_t count, const std::shared_ptr<RenderContext> &context) {
        WGPUBufferDescriptor descriptor{
            .label = m_Label.c_str(),
            .usage = static_cast<WGPUBufferUsageFlags>(m_BufferUsage) | WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform,
            .size = sizeof(float) * count,
        };

        WGPUBuffer buffer = wgpuDeviceCreateBuffer(context->getDevice(), &descriptor);

        return std::make_shared<UniformBuffer>(m_Label, buffer);
    }

    std::shared_ptr<UniformBuffer> UniformBufferBuilder::build(const void *data, size_t size, const std::shared_ptr<RenderContext> &context) {
        WGPUBufferDescriptor descriptor{
            .label = m_Label.c_str(),
            .usage = static_cast<WGPUBufferUsageFlags>(m_BufferUsage) | WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform,
            .size = size,
        };

        WGPUBuffer buffer = wgpuDeviceCreateBufferInit(context->getDevice(), descriptor, data);

        return std::make_shared<UniformBuffer>(m_Label, buffer);
    }
}  // namespace Yulduz