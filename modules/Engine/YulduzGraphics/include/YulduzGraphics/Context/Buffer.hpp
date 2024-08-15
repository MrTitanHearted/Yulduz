#pragma once

#include <Yulduz/Core.hpp>
#include <YulduzGraphics/Enums.hpp>
#include <YulduzGraphics/Context/GraphicsContext.hpp>

namespace Yulduz {
    class Buffer {
       public:
        Buffer(const std::string &label, const WGPUBuffer &handle, const GraphicsContext &context);

        Buffer();
        ~Buffer();

        Buffer(const Buffer &other);
        Buffer(Buffer &&other);

        Buffer &operator=(const Buffer &other);
        Buffer &operator=(Buffer &&other);

        void write(void *data) const;
        void write(void *data, std::size_t size) const;
        void writeWithOffset(void *data, std::size_t offset) const;
        void writeWithOffset(void *data, std::size_t size, std::size_t offset) const;

        std::string getLabel() const;
        WGPUBuffer get() const;
        std::size_t getSize() const;
        BufferUsage getUsage() const;

       protected:
        std::string m_Label;
        WGPUBuffer m_Buffer;

        GraphicsContext m_ContextReference;
    };

    class VertexBuffer : public Buffer {
       public:
        VertexBuffer(const std::string &label, const WGPUBuffer &handle, std::size_t count, std::size_t stride, const GraphicsContext &context);

        VertexBuffer() = default;
        ~VertexBuffer() = default;

        VertexBuffer(const VertexBuffer &other);
        VertexBuffer(VertexBuffer &&other);

        VertexBuffer &operator=(const VertexBuffer &other);
        VertexBuffer &operator=(VertexBuffer &&other);

        std::size_t getCount() const;
        std::size_t getStride() const;

       private:
        std::size_t m_Count;
        std::size_t m_Stride;
    };

    class IndexBuffer : public Buffer {
       public:
        IndexBuffer(std::string &label, const WGPUBuffer &handle, std::size_t count, IndexFormat indexFormat, const GraphicsContext &context);

        IndexBuffer() = default;
        ~IndexBuffer() = default;

        IndexBuffer(const IndexBuffer &other);
        IndexBuffer(IndexBuffer &&other);

        IndexBuffer &operator=(const IndexBuffer &other);
        IndexBuffer &operator=(IndexBuffer &&other);

        std::size_t getCount() const;
        IndexFormat getIndexFormat() const;

       private:
        std::size_t m_Count;
        IndexFormat m_IndexFormat;
    };

    class UniformBuffer : public Buffer {
       public:
        UniformBuffer(const std::string &label, const WGPUBuffer &handle, const GraphicsContext &context);

        UniformBuffer() = default;
        ~UniformBuffer() = default;

        UniformBuffer(const UniformBuffer &other);
        UniformBuffer(UniformBuffer &&other);

        UniformBuffer &operator=(const UniformBuffer &other);
        UniformBuffer &operator=(UniformBuffer &&other);
    };

    class StorageBuffer : public Buffer {
       public:
        StorageBuffer(const std::string &label, const WGPUBuffer &handle, const GraphicsContext &context);

        StorageBuffer() = default;
        ~StorageBuffer() = default;

        StorageBuffer(const StorageBuffer &other);
        StorageBuffer(StorageBuffer &&other);

        StorageBuffer &operator=(const StorageBuffer &other);
        StorageBuffer &operator=(StorageBuffer &&other);
    };

    class BufferBuilder {
       public:
        BufferBuilder() = default;
        ~BufferBuilder() = default;

        inline static BufferBuilder New() { return BufferBuilder(); }

        BufferBuilder &setLabel(const std::string &label);
        BufferBuilder &setBufferUsage(BufferUsage usage);
        BufferBuilder &addBufferUsage(BufferUsage usage);

        Buffer empty(std::size_t size, const GraphicsContext &context);
        Buffer build(const void *data, std::size_t size, const GraphicsContext &context);

        IndexBuffer emptyIndexU16(std::size_t count, const GraphicsContext &context);
        IndexBuffer emptyIndexU32(std::size_t count, const GraphicsContext &context);
        IndexBuffer buildIndex(const std::vector<std::uint16_t> &indices, const GraphicsContext &context);
        IndexBuffer buildIndex(const std::vector<std::uint32_t> &indices, const GraphicsContext &context);

        UniformBuffer emptyUniform(std::size_t size, const GraphicsContext &context);
        UniformBuffer buildUniform(const void *data, std::size_t size, const GraphicsContext &context);

        StorageBuffer emptyStorage(std::size_t size, const GraphicsContext &context);
        StorageBuffer buildStorage(const void *data, std::size_t size, const GraphicsContext &context);

        template <typename Vertex>
        VertexBuffer emptyVertex(std::size_t count, const GraphicsContext &context) {
            return emptyVertex(sizeof(Vertex) * count, count, sizeof(Vertex), context);
        }
        template <typename Vertex>
        VertexBuffer buildVertex(const std::vector<Vertex> &vertices, const GraphicsContext &context) {
            return buildVertex(vertices.data(), sizeof(Vertex) * vertices.size(), vertices.size(), sizeof(Vertex), context);
        }

       private:
        std::string m_Label;
        BufferUsage m_BufferUsage;

        VertexBuffer emptyVertex(std::size_t size, std::size_t count, std::size_t stride, const GraphicsContext &context);
        VertexBuffer buildVertex(const void *data, std::size_t size, std::size_t count, std::size_t stride, const GraphicsContext &context);
    };

    WGPUBuffer wgpuDeviceCreateBufferInit(WGPUDevice device, WGPUBufferDescriptor &descriptor, const void *data);
}  // namespace Yulduz
