#include <YulduzGraphics/Context/Sampler.hpp>
#include <YulduzGraphics/Context/GraphicsContext.hpp>

namespace Yulduz {
    Sampler::Sampler(const std::string &label, const WGPUSampler &sampler) {
        assert(sampler != nullptr && "Sampler handler cannot be nullptr");

        m_Sampler = sampler;
        m_Label = label;
    }

    Sampler::~Sampler() {
        if (m_Sampler)
            wgpuSamplerRelease(m_Sampler);
    }

    Sampler::Sampler(const Sampler &other) {
        assert(other.m_Sampler != nullptr && "Sampler handler cannot be nullptr");

        if (&other != this) {
            wgpuSamplerRelease(m_Sampler);

            m_Sampler = other.m_Sampler;
            m_Label = other.m_Label;

            wgpuSamplerReference(m_Sampler);
        }
    }

    Sampler &Sampler::operator=(const Sampler &other) {
        assert(other.m_Sampler != nullptr && "Sampler handler cannot be nullptr");

        if (&other != this) {
            wgpuSamplerRelease(m_Sampler);

            m_Sampler = other.m_Sampler;
            m_Label = other.m_Label;

            wgpuSamplerReference(m_Sampler);
        }

        return *this;
    }

    Sampler::Sampler(Sampler &&other) {
        assert(other.m_Sampler != nullptr && "Sampler handler cannot be nullptr");

        if (&other != this) {
            wgpuSamplerRelease(m_Sampler);

            m_Sampler = other.m_Sampler;
            m_Label = other.m_Label;

            other.m_Sampler = nullptr;
            other.m_Label = "";
        }
    }

    Sampler &Sampler::operator=(Sampler &&other) {
        assert(other.m_Sampler != nullptr && "Sampler handler cannot be nullptr");

        if (&other != this) {
            wgpuSamplerRelease(m_Sampler);

            m_Sampler = other.m_Sampler;
            m_Label = other.m_Label;

            other.m_Sampler = nullptr;
            other.m_Label = "";
        }

        return *this;
    }

    WGPUSampler Sampler::get() const {
        assert(m_Sampler != nullptr && "Sampler handler cannot be nullptr");

        return m_Sampler;
    }

    std::string Sampler::getLabel() const {
        assert(m_Sampler != nullptr && "Sampler handler cannot be nullptr");

        return m_Label;
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

    SamplerBuilder &SamplerBuilder::setMaxAnisotropy(std::uint16_t max) {
        m_Descriptor.maxAnisotropy = max;
        return *this;
    }

    Sampler SamplerBuilder::build(const GraphicsContext &context) {
        m_Descriptor.label = m_Label.c_str();
        return Sampler{m_Label, wgpuDeviceCreateSampler(context.getDevice(), &m_Descriptor)};
    }
}  // namespace Yulduz
