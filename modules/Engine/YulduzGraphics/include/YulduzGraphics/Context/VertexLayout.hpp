#pragma once

#include <Yulduz/Core.hpp>
#include <YulduzGraphics/Enums.hpp>

namespace Yulduz {
    class VertexLayout {
       public:
        struct Attribute {
            VertexFormat Format;
            std::uint64_t Offset;
            std::uint32_t ShaderLocation;
        };

       public:
        VertexLayout(std::size_t arrayStride);
        ~VertexLayout() = default;

        inline static VertexLayout New(std::size_t arrayStride) { return VertexLayout(arrayStride); }

        VertexLayout &setStepMode(VertexStepMode mode);
        VertexLayout &setAttributes(const std::vector<Attribute> &attributes);
        VertexLayout &addAttribute(VertexFormat format, std::uint64_t offset, std::uint32_t shaderLocation);

        static WGPUVertexBufferLayout Get(const VertexLayout &layout);

       private:
        std::vector<Attribute> m_Attributes;
        VertexStepMode m_StepMode;
        std::size_t m_ArrayStride;
    };
}  // namespace Yulduz
