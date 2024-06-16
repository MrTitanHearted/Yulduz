#include <YulduzRender/Context/VertexLayout.hpp>

namespace Yulduz {
    VertexLayout::VertexLayout(size_t arrayStride) {
        m_ArrayStride = arrayStride;
        m_StepMode = VertexStepMode::Vertex;
    }

    VertexLayout &VertexLayout::setStepMode(VertexStepMode mode) {
        m_StepMode = mode;
        return *this;
    }

    VertexLayout &VertexLayout::setAttributes(const std::vector<Attribute> &attributes) {
        m_VertexAttributes.resize(attributes.size());
        for (size_t i = 0; i < attributes.size(); i++) {
            m_VertexAttributes[i] = WGPUVertexAttribute{
                .format = static_cast<WGPUVertexFormat>(attributes[i].format),
                .offset = attributes[i].offset,
                .shaderLocation = attributes[i].shaderLocation,
            };
        }
        return *this;
    }

    VertexLayout &VertexLayout::addAttributes(const std::vector<Attribute> &attributes) {
        for (const Attribute &attribute : attributes) {
            m_VertexAttributes.emplace_back(WGPUVertexAttribute{
                .format = static_cast<WGPUVertexFormat>(attribute.format),
                .offset = attribute.offset,
                .shaderLocation = attribute.shaderLocation,
            });
        }
        return *this;
    }

    VertexLayout &VertexLayout::addAttribute(VertexFormat format, size_t offset, uint32_t shaderLocation) {
        m_VertexAttributes.emplace_back(WGPUVertexAttribute{
            .format = static_cast<WGPUVertexFormat>(format),
            .offset = offset,
            .shaderLocation = shaderLocation,
        });
        return *this;
    }

    WGPUVertexBufferLayout VertexLayout::Get(const VertexLayout &layout) {
        return WGPUVertexBufferLayout{
            .arrayStride = layout.m_ArrayStride,
            .stepMode = static_cast<WGPUVertexStepMode>(layout.m_StepMode),
            .attributeCount = static_cast<uint32_t>(layout.m_VertexAttributes.size()),
            .attributes = layout.m_VertexAttributes.data(),
        };
    }
    
}  // namespace Yulduz