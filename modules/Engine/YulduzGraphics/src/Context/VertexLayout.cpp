#include <YulduzGraphics/Context/VertexLayout.hpp>

namespace Yulduz {
    VertexLayout::VertexLayout(std::size_t arrayStride) {
        m_ArrayStride = arrayStride;
        m_StepMode = VertexStepMode::Vertex;
    }

    VertexLayout &VertexLayout::setStepMode(VertexStepMode mode) {
        m_StepMode = mode;
        return *this;
    }

    VertexLayout &VertexLayout::setAttributes(const std::vector<Attribute> &attributes) {
        m_Attributes = attributes;
        return *this;
    }

    VertexLayout &VertexLayout::addAttribute(VertexFormat format, std::uint64_t offset, std::uint32_t shaderLocation) {
        m_Attributes.push_back(Attribute{
            .Format = format,
            .Offset = offset,
            .ShaderLocation = shaderLocation,
        });
        return *this;
    }

    WGPUVertexBufferLayout VertexLayout::Get(const VertexLayout &layout) {
        return WGPUVertexBufferLayout{
            .arrayStride = layout.m_ArrayStride,
            .stepMode = static_cast<WGPUVertexStepMode>(layout.m_StepMode),
            .attributeCount = layout.m_Attributes.size(),
            .attributes = (WGPUVertexAttribute *)layout.m_Attributes.data(),
        };
    }
}  // namespace Yulduz
