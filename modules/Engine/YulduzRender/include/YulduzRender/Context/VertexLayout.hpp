#pragma once

#include <Yulduz/Core.hpp>
#include <YulduzRender/Enums.hpp>

namespace Yulduz {
    class VertexLayout {
       public:
        struct Attribute {
            VertexFormat format;
            std::size_t offset;
            std::uint32_t shaderLocation;
        };

       public:
        VertexLayout(std::size_t arrayStride);
        ~VertexLayout() = default;

        VertexLayout &setStepMode(VertexStepMode mode);
        VertexLayout &setAttributes(const std::vector<Attribute> &attributes);
        VertexLayout &addAttributes(const std::vector<Attribute> &attributes);
        VertexLayout &addAttribute(VertexFormat format, std::size_t offset, std::uint32_t shaderLocation);

        static WGPUVertexBufferLayout Get(const VertexLayout &layout);

       private:
        std::vector<WGPUVertexAttribute> m_VertexAttributes;
        VertexStepMode m_StepMode;
        std::size_t m_ArrayStride;
    };
}  // namespace Yulduz