#pragma once

#include <Yulduz/Core.hpp>
#include <YulduzRender/Enums.hpp>
#include <YulduzRender/Context/Buffer.hpp>
#include <YulduzRender/Context/VertexLayout.hpp>

namespace Yulduz {
    class RenderBuffer;

    class VertexBuffer : public Buffer {
       public:
        VertexBuffer(const std::string &label, const WGPUBuffer &buffer, size_t count, size_t stride);
        ~VertexBuffer();

        size_t getCount() const;
        size_t getStride() const;

       private:
        size_t m_Count;
        size_t m_Stride;
    };

    class IndexBuffer : public Buffer {
       public:
        IndexBuffer(const std::string &label, const WGPUBuffer &buffer, size_t count, IndexFormat indexFormat);
        ~IndexBuffer();

        size_t getCount() const;
        IndexFormat getIndexFormat() const;

       private:
        size_t m_Count;
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

    class VertexBufferBuilder {
       public:
        VertexBufferBuilder();
        ~VertexBufferBuilder() = default;

        inline static VertexBufferBuilder New() { return VertexBufferBuilder(); }

        VertexBufferBuilder &setLabel(const std::string &label);
        VertexBufferBuilder &setBufferUsage(BufferUsage usage);
        VertexBufferBuilder &addBufferUsage(BufferUsage usage);

        template <typename Vertex>
        std::shared_ptr<VertexBuffer> empty(size_t count, const std::shared_ptr<RenderContext> &context) {
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
        std::shared_ptr<VertexBuffer> empty(size_t size, size_t count, size_t stride, const std::shared_ptr<RenderContext> &context);
        std::shared_ptr<VertexBuffer> build(const void *data, size_t size, size_t count, size_t stride, const std::shared_ptr<RenderContext> &context);
    };

    class IndexBufferBuilder {
       public:
        IndexBufferBuilder();
        ~IndexBufferBuilder() = default;

        inline static IndexBufferBuilder New() { return IndexBufferBuilder(); }

        IndexBufferBuilder &setLabel(const std::string &label);
        IndexBufferBuilder &setBufferUsage(BufferUsage usage);
        IndexBufferBuilder &addBufferUsage(BufferUsage usage);

        std::shared_ptr<IndexBuffer> emptyU16(size_t count, const std::shared_ptr<RenderContext> &context);
        std::shared_ptr<IndexBuffer> emptyU32(size_t count, const std::shared_ptr<RenderContext> &context);

        std::shared_ptr<IndexBuffer> build(const std::vector<uint16_t> &indices, const std::shared_ptr<RenderContext> &context);
        std::shared_ptr<IndexBuffer> build(const std::vector<uint32_t> &indices, const std::shared_ptr<RenderContext> &context);

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

        std::shared_ptr<StorageBuffer> empty(size_t size, const std::shared_ptr<RenderContext> &context);
        std::shared_ptr<StorageBuffer> build(const void *data, size_t size, const std::shared_ptr<RenderContext> &context);

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

        std::shared_ptr<UniformBuffer> empty(size_t size, const std::shared_ptr<RenderContext> &context);
        std::shared_ptr<UniformBuffer> build(const void *data, size_t size, const std::shared_ptr<RenderContext> &context);

       private:
        std::string m_Label;
        BufferUsage m_BufferUsage;
    };

}  // namespace Yulduz