#include <YulduzRender/Context/Sampler.hpp>
#include <YulduzRender/Context.hpp>

namespace Yulduz {
    Sampler::Sampler(const std::string &label, const WGPUSampler &sampler) {
        YZDEBUG("Initializing Sampler: '{}'", label);

        m_Label = label;
        m_Sampler = sampler;
    }

    Sampler::~Sampler() {
        YZDEBUG("Releasing Sampler: '{}'", m_Label);

        wgpuSamplerRelease(m_Sampler);
    }

    std::string Sampler::getLabel() const {
        return m_Label;
    }

    WGPUSampler Sampler::get() const {
        return m_Sampler;
    }

    SamplerBuilder::SamplerBuilder() {
        m_Label = "Yulduz Sampler";
        m_Descriptor = WGPUSamplerDescriptor{
            .addressModeU = WGPUAddressMode_Repeat,
            .addressModeV = WGPUAddressMode_Repeat,
            .addressModeW = WGPUAddressMode_Repeat,
            .magFilter = WGPUFilterMode_Linear,
            .minFilter = WGPUFilterMode_Linear,
            .mipmapFilter = WGPUMipmapFilterMode_Linear,
            .lodMinClamp = 0.0f,
            .lodMaxClamp = 1.0f,
            .compare = WGPUCompareFunction_Undefined,
            .maxAnisotropy = 1,
        };
    }

    SamplerBuilder &SamplerBuilder::setLabel(const std::string &label) {
        m_Label = label;
        return *this;
    }

    SamplerBuilder &SamplerBuilder::setAddressMode(AddressMode mode) {
        m_Descriptor.addressModeU = static_cast<WGPUAddressMode>(mode);
        m_Descriptor.addressModeV = static_cast<WGPUAddressMode>(mode);
        m_Descriptor.addressModeW = static_cast<WGPUAddressMode>(mode);
        return *this;
    }

    SamplerBuilder &SamplerBuilder::setAddressModeU(AddressMode mode) {
        m_Descriptor.addressModeU = static_cast<WGPUAddressMode>(mode);
        return *this;
    }

    SamplerBuilder &SamplerBuilder::setAddressModeV(AddressMode mode) {
        m_Descriptor.addressModeV = static_cast<WGPUAddressMode>(mode);
        return *this;
    }

    SamplerBuilder &SamplerBuilder::setAddressModeW(AddressMode mode) {
        m_Descriptor.addressModeW = static_cast<WGPUAddressMode>(mode);
        return *this;
    }

    SamplerBuilder &SamplerBuilder::setMinMagFilter(FilterMode mode) {
        m_Descriptor.magFilter = static_cast<WGPUFilterMode>(mode);
        m_Descriptor.minFilter = static_cast<WGPUFilterMode>(mode);
        return *this;
    }

    SamplerBuilder &SamplerBuilder::setMagFilter(FilterMode mode) {
        m_Descriptor.magFilter = static_cast<WGPUFilterMode>(mode);
        return *this;
    }

    SamplerBuilder &SamplerBuilder::setMinFilter(FilterMode mode) {
        m_Descriptor.minFilter = static_cast<WGPUFilterMode>(mode);
        return *this;
    }

    SamplerBuilder &SamplerBuilder::setMipmapFilter(MipmapFilterMode mode) {
        m_Descriptor.mipmapFilter = static_cast<WGPUMipmapFilterMode>(mode);
        return *this;
    }

    SamplerBuilder &SamplerBuilder::setLodMinClamp(float min) {
        m_Descriptor.lodMaxClamp = min;
        return *this;
    }

    SamplerBuilder &SamplerBuilder::setLodMaxClamp(float max) {
        m_Descriptor.lodMaxClamp = max;
        return *this;
    }

    SamplerBuilder &SamplerBuilder::setCompareFunction(CompareFunction compare) {
        m_Descriptor.compare = static_cast<WGPUCompareFunction>(compare);
        return *this;
    }

    SamplerBuilder &SamplerBuilder::setMaxAnisotropy(uint16_t max) {
        m_Descriptor.maxAnisotropy = max;
        return *this;
    }

    std::shared_ptr<Sampler> SamplerBuilder::build(const std::shared_ptr<RenderContext> &context) {
        m_Descriptor.label = m_Label.c_str();
        WGPUSampler sampler = wgpuDeviceCreateSampler(context->getDevice(), &m_Descriptor);
        return std::make_shared<Sampler>(m_Label, sampler);
    }
}  // namespace Yulduz
