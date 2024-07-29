#pragma once

#include <Yulduz/Core.hpp>
#include <YulduzRender/Enums.hpp>

namespace Yulduz {
    class RenderContext;

    class Buffer {
       public:
        Buffer(const std::string &label, const WGPUBuffer &buffer);
        ~Buffer();

        void write(void *data, const std::shared_ptr<RenderContext> &context) const;
        void write(void *data, std::size_t size, const std::shared_ptr<RenderContext> &context) const;
        void writeWithOffset(void *data, std::size_t offset, const std::shared_ptr<RenderContext> &context) const;
        void writeWithOffset(void *data, std::size_t offset, std::size_t size, const std::shared_ptr<RenderContext> &context) const;

        std::string getLabel() const;
        WGPUBuffer get() const;
        std::size_t getSize() const;
        BufferUsage getUsage() const;

       private:
        std::string m_Label;
        WGPUBuffer m_Buffer;
        bool m_IsDefaultBuffer;

       private:
        Buffer() = default;

        friend class VertexBuffer;
        friend class IndexBuffer;
        friend class StorageBuffer;
        friend class UniformBuffer;
    };

    class VertexBuffer : public Buffer {
       public:
        VertexBuffer(const std::string &label, const WGPUBuffer &buffer, std::size_t count, std::size_t stride);
        ~VertexBuffer();

        std::size_t getCount() const;
        std::size_t getStride() const;

       private:
        std::size_t m_Count;
        std::size_t m_Stride;
    };

    class IndexBuffer : public Buffer {
       public:
        IndexBuffer(const std::string &label, const WGPUBuffer &buffer, std::size_t count, IndexFormat indexFormat);
        ~IndexBuffer();

        std::size_t getCount() const;
        IndexFormat getIndexFormat() const;

       private:
        std::size_t m_Count;
        IndexFormat m_IndexFormat;
    };

    class UniformBuffer : public Buffer {
       public:
        UniformBuffer(const std::string &label, const WGPUBuffer &buffer);
        ~UniformBuffer();
    };

    class StorageBuffer : public Buffer {
       public:
        StorageBuffer(const std::string &label, const WGPUBuffer &buffer);
        ~StorageBuffer();
    };

    class BufferBuilder {
       public:
        BufferBuilder();
        ~BufferBuilder() = default;

        inline static BufferBuilder New() { return BufferBuilder(); }

        BufferBuilder &setLabel(const std::string &label);
        BufferBuilder &setBufferUsageReq(BufferUsage usage);
        BufferBuilder &addBufferUsage(BufferUsage usage);

        std::shared_ptr<Buffer> empty(std::size_t size, const std::shared_ptr<RenderContext> &context);
        std::shared_ptr<Buffer> build(const void *data, std::size_t size, const std::shared_ptr<RenderContext> &context);

       private:
        std::string m_Label;
        BufferUsage m_BufferUsage;
    };

    class VertexBufferBuilder {
       public:
        VertexBufferBuilder();
        ~VertexBufferBuilder() = default;

        inline static VertexBufferBuilder New() { return VertexBufferBuilder(); }

        VertexBufferBuilder &setLabel(const std::string &label);
        VertexBufferBuilder &setBufferUsage(BufferUsage usage);
        VertexBufferBuilder &addBufferUsage(BufferUsage usage);

        template <typename Vertex>
        std::shared_ptr<VertexBuffer> empty(std::size_t count, const std::shared_ptr<RenderContext> &context) {
            return empty(sizeof(Vertex) * count, count, sizeof(Vertex), context);
        }

        template <typename Vertex>
        std::shared_ptr<VertexBuffer> build(const std::vector<Vertex> &vertices, const std::shared_ptr<RenderContext> &context) {
            return build(vertices.data(), sizeof(Vertex) * vertices.size(), vertices.size(), sizeof(Vertex), context);
        }

       private:
        std::string m_Label;
        BufferUsage m_BufferUsage;

       private:
        std::shared_ptr<VertexBuffer> empty(std::size_t size, std::size_t count, std::size_t stride, const std::shared_ptr<RenderContext> &context);
        std::shared_ptr<VertexBuffer> build(const void *data, std::size_t size, std::size_t count, std::size_t stride, const std::shared_ptr<RenderContext> &context);
    };

    class IndexBufferBuilder {
       public:
        IndexBufferBuilder();
        ~IndexBufferBuilder() = default;

        inline static IndexBufferBuilder New() { return IndexBufferBuilder(); }

        IndexBufferBuilder &setLabel(const std::string &label);
        IndexBufferBuilder &setBufferUsage(BufferUsage usage);
        IndexBufferBuilder &addBufferUsage(BufferUsage usage);

        std::shared_ptr<IndexBuffer> emptyU16(std::size_t count, const std::shared_ptr<RenderContext> &context);
        std::shared_ptr<IndexBuffer> emptyU32(std::size_t count, const std::shared_ptr<RenderContext> &context);

        std::shared_ptr<IndexBuffer> build(const std::vector<std::uint16_t> &indices, const std::shared_ptr<RenderContext> &context);
        std::shared_ptr<IndexBuffer> build(const std::vector<std::uint32_t> &indices, const std::shared_ptr<RenderContext> &context);

       private:
        std::string m_Label;
        BufferUsage m_BufferUsage;
    };

    class StorageBufferBuilder {
       public:
        StorageBufferBuilder();
        ~StorageBufferBuilder() = default;

        inline static StorageBufferBuilder New() { return StorageBufferBuilder(); }

        StorageBufferBuilder &setLabel(const std::string &label);
        StorageBufferBuilder &setBufferUsage(BufferUsage usage);
        StorageBufferBuilder &addBufferUsage(BufferUsage usage);

        std::shared_ptr<StorageBuffer> empty(std::size_t size, const std::shared_ptr<RenderContext> &context);
        std::shared_ptr<StorageBuffer> build(const void *data, std::size_t size, const std::shared_ptr<RenderContext> &context);

       private:
        std::string m_Label;
        BufferUsage m_BufferUsage;
    };

    class UniformBufferBuilder {
       public:
        UniformBufferBuilder();
        ~UniformBufferBuilder() = default;

        inline static UniformBufferBuilder New() { return UniformBufferBuilder(); }

        UniformBufferBuilder &setLabel(const std::string &label);
        UniformBufferBuilder &setBufferUsage(BufferUsage usage);
        UniformBufferBuilder &addBufferUsage(BufferUsage usage);

        std::shared_ptr<UniformBuffer> empty(std::size_t size, const std::shared_ptr<RenderContext> &context);
        std::shared_ptr<UniformBuffer> build(const void *data, std::size_t size, const std::shared_ptr<RenderContext> &context);

       private:
        std::string m_Label;
        BufferUsage m_BufferUsage;
    };

    WGPUBuffer wgpuDeviceCreateBufferInit(WGPUDevice device, WGPUBufferDescriptor descriptor, const void *data);
}  // namespace Yulduz
