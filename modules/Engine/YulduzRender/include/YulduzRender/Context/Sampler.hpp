#pragma once

#include <Yulduz/Core.hpp>
#include <YulduzRender/Enums.hpp>

namespace Yulduz {
    class RenderContext;

    class Sampler {
       public:
        Sampler(const std::string &label, const WGPUSampler &sampler);
        ~Sampler();

        std::string getLabel() const;
        WGPUSampler get() const;

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
        SamplerBuilder &setMaxAnisotropy(uint16_t max);

        std::shared_ptr<Sampler> build(const std::shared_ptr<RenderContext> &context);

       private:
        std::string m_Label;
        WGPUSamplerDescriptor m_Descriptor;
    };
}  // namespace Yulduz
