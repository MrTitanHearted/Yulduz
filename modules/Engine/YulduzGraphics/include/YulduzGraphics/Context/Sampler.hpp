#pragma once

#include <Yulduz/Core.hpp>
#include <YulduzGraphics/Enums.hpp>

namespace Yulduz {
    class GraphicsContext;

    class Sampler {
       public:
        Sampler(const std::string &label, const WGPUSampler &sampler);

        Sampler();
        ~Sampler();

        Sampler(const Sampler &other);
        Sampler(Sampler &&other);

        Sampler &operator=(const Sampler &other);
        Sampler &operator=(Sampler &&other);

        WGPUSampler get() const;
        std::string getLabel() const;

       private:
        std::string m_Label;
        WGPUSampler m_Sampler;
    };

    class SamplerBuilder {
       public:
        SamplerBuilder();
        ~SamplerBuilder() = default;

        inline static SamplerBuilder New() { return SamplerBuilder(); }

        SamplerBuilder &setLabel(const std::string &label);
        SamplerBuilder &setAddressMode(AddressMode mode);
        SamplerBuilder &setAddressModeU(AddressMode mode);
        SamplerBuilder &setAddressModeV(AddressMode mode);
        SamplerBuilder &setAddressModeW(AddressMode mode);
        SamplerBuilder &setMinMagFilter(FilterMode mode);
        SamplerBuilder &setMagFilter(FilterMode mode);
        SamplerBuilder &setMinFilter(FilterMode mode);
        SamplerBuilder &setMipmapFilter(MipmapFilterMode mode);
        SamplerBuilder &setLodMinClamp(float min);
        SamplerBuilder &setLodMaxClamp(float max);
        SamplerBuilder &setCompareFunction(CompareFunction compare);
        SamplerBuilder &setMaxAnisotropy(std::uint16_t max);

        Sampler build(const GraphicsContext &context);

       private:
        std::string m_Label;
        WGPUSamplerDescriptor m_Descriptor;
    };
}  // namespace Yulduz