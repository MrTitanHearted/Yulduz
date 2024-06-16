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
        void write(void *data, size_t size, const std::shared_ptr<RenderContext> &context) const;
        void writeWithOffset(void *data, size_t offset, const std::shared_ptr<RenderContext> &context) const;
        void writeWithOffset(void *data, size_t offset, size_t size, const std::shared_ptr<RenderContext> &context) const;

        std::string getLabel() const;
        WGPUBuffer get() const;
        size_t getSize() const;
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

    class BufferBuilder {
       public:
        BufferBuilder();
        ~BufferBuilder() = default;

        inline static BufferBuilder New() { return BufferBuilder(); }

        BufferBuilder &setLabel(const std::string &label);
        BufferBuilder &setBufferUsage(BufferUsage usage);
        BufferBuilder &addBufferUsage(BufferUsage usage);

        std::shared_ptr<Buffer> empty(size_t size, const std::shared_ptr<RenderContext> &context);
        std::shared_ptr<Buffer> build(const void *data, size_t size, const std::shared_ptr<RenderContext> &context);

       private:
        std::string m_Label;
        BufferUsage m_BufferUsage;
    };

    WGPUBuffer wgpuDeviceCreateBufferInit(WGPUDevice device, WGPUBufferDescriptor descriptor, const void *data);
}  // namespace Yulduz