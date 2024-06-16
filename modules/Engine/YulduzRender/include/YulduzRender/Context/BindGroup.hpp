#pragma once

#include <Yulduz/Core.hpp>
#include <YulduzRender/Enums.hpp>
#include <YulduzRender/Context/Buffer.hpp>
#include <YulduzRender/Context/Buffers.hpp>
#include <YulduzRender/Context/Sampler.hpp>
#include <YulduzRender/Context/Texture.hpp>

namespace Yulduz {
    class BufferEntry {
       public:
        BufferEntry(ShaderStage stage);
        ~BufferEntry() = default;

        BufferEntry &setShaderStage(ShaderStage stage);

        WGPUBindGroupLayoutEntry getStorage(uint32_t binding) const;
        WGPUBindGroupLayoutEntry getUniform(uint32_t binding) const;

       private:
        ShaderStage m_ShaderStage;
    };

    class SamplerEntry {
       public:
        SamplerEntry(ShaderStage stage);
        ~SamplerEntry() = default;

        SamplerEntry &setShaderStage(ShaderStage stage);
        SamplerEntry &setBindingType(SamplerBindingType type);

        WGPUBindGroupLayoutEntry get(uint32_t binding) const;

       private:
        ShaderStage m_ShaderStage;
        SamplerBindingType m_SamplerBindingType;
    };

    class TextureEntry {
       public:
        TextureEntry(ShaderStage stage);
        ~TextureEntry() = default;

        TextureEntry &setShaderStage(ShaderStage stage);
        TextureEntry &setTextureSampleType(TextureSampleType type);
        TextureEntry &setIsMultisampled(bool isMultisampled);

        WGPUBindGroupLayoutEntry get2d(uint32_t binding) const;
        WGPUBindGroupLayoutEntry get3d(uint32_t binding) const;

       private:
        ShaderStage m_ShaderStage;
        TextureSampleType m_TextureSampleType;
        bool m_IsMultisampled;
    };

    class StorageTextureEntry {
       public:
        StorageTextureEntry(ShaderStage stage, TextureFormat format);
        ~StorageTextureEntry() = default;

        StorageTextureEntry &setShaderStage(ShaderStage stage);
        StorageTextureEntry &setStorageTextureAccess(StorageTextureAccess access);
        StorageTextureEntry &setTextureFormat(TextureFormat format);

        WGPUBindGroupLayoutEntry get2d(uint32_t binding) const;
        WGPUBindGroupLayoutEntry get3d(uint32_t binding) const;

       private:
        ShaderStage m_ShaderStage;
        StorageTextureAccess m_StorageTextureAccess;
        TextureFormat m_TextureFormat;
    };

    class BindGroupLayout {
       public:
        BindGroupLayout(const std::string &label, const WGPUBindGroupLayout &bindGroupLayout);
        ~BindGroupLayout();

        std::string getLabel() const;
        WGPUBindGroupLayout get() const;

       private:
        std::string m_Label;
        WGPUBindGroupLayout m_BindGroupLayout;
    };

    class BindGroup {
       public:
        BindGroup(const std::string &label,
                  const WGPUBindGroup &bindGroup,
                  const std::shared_ptr<BindGroupLayout> &bindGroupLayout,
                  const std::vector<std::shared_ptr<Buffer>> &buffers,
                  const std::vector<std::shared_ptr<Sampler>> &samplers,
                  const std::vector<std::shared_ptr<Texture>> &textures);
        ~BindGroup();

        std::string getLabel() const;
        WGPUBindGroup get() const;
        std::shared_ptr<BindGroupLayout> getLayout() const;

       private:
        std::string m_Label;
        WGPUBindGroup m_BindGroup;
        std::shared_ptr<BindGroupLayout> m_BindGroupLayout;
        std::vector<std::shared_ptr<Buffer>> m_Buffers;
        std::vector<std::shared_ptr<Sampler>> m_Samplers;
        std::vector<std::shared_ptr<Texture>> m_Textures;
    };

    class BindGroupLayoutBuilder {
       public:
        BindGroupLayoutBuilder();
        ~BindGroupLayoutBuilder() = default;

        inline static BindGroupLayoutBuilder New() { return BindGroupLayoutBuilder(); }

        BindGroupLayoutBuilder &setLabel(const std::string &label);
        BindGroupLayoutBuilder &addStorageBuffer(uint32_t binding, const BufferEntry &entry);
        BindGroupLayoutBuilder &addUniformBuffer(uint32_t binding, const BufferEntry &entry);
        BindGroupLayoutBuilder &addSampler(uint32_t binding, const SamplerEntry &entry);
        BindGroupLayoutBuilder &addTexture2D(uint32_t binding, const TextureEntry &entry);
        BindGroupLayoutBuilder &addTexture3D(uint32_t binding, const TextureEntry &entry);
        BindGroupLayoutBuilder &addStorageTexture2D(uint32_t binding, const StorageTextureEntry &entry);
        BindGroupLayoutBuilder &addStorageTexture3D(uint32_t binding, const StorageTextureEntry &entry);

        inline BindGroupLayoutBuilder &addStorageBuffer(uint32_t binding) {
            return addStorageBuffer(binding, BufferEntry(ShaderStage::Vertex | ShaderStage::Fragment | ShaderStage::Compute));
        }
        inline BindGroupLayoutBuilder &addUniformBuffer(uint32_t binding) {
            return addUniformBuffer(binding, BufferEntry(ShaderStage::Vertex | ShaderStage::Fragment | ShaderStage::Compute));
        }
        inline BindGroupLayoutBuilder &addSampler(uint32_t binding) {
            return addSampler(binding, SamplerEntry(ShaderStage::Fragment));
        }
        inline BindGroupLayoutBuilder &addTexture2D(uint32_t binding) {
            return addTexture2D(binding, TextureEntry(ShaderStage::Fragment));
        }
        inline BindGroupLayoutBuilder &addTexture3D(uint32_t binding) {
            return addTexture3D(binding, TextureEntry(ShaderStage::Fragment));
        }
        inline BindGroupLayoutBuilder &addStorageTexture2D(uint32_t binding) {
            return addStorageTexture2D(binding, StorageTextureEntry(ShaderStage::Fragment | ShaderStage::Compute, TextureFormat::RGBA8Unorm));
        }
        inline BindGroupLayoutBuilder &addStorageTexture3D(uint32_t binding) {
            return addStorageTexture3D(binding, StorageTextureEntry(ShaderStage::Fragment | ShaderStage::Compute, TextureFormat::RGBA8Unorm));
        }

        std::shared_ptr<BindGroupLayout> build(const std::shared_ptr<RenderContext> &context);

       private:
        std::string m_Label;
        std::vector<WGPUBindGroupLayoutEntry> m_Entries;
    };

    class BindGroupBuilder {
       public:
        BindGroupBuilder();
        ~BindGroupBuilder() = default;

        inline static BindGroupBuilder New() { return BindGroupBuilder(); }

        BindGroupBuilder &setLabel(const std::string &label);
        BindGroupBuilder &addStorageBuffer(uint32_t binding, const std::shared_ptr<StorageBuffer> &buffer);
        BindGroupBuilder &addUniformBuffer(uint32_t binding, const std::shared_ptr<UniformBuffer> &buffer);
        BindGroupBuilder &addSampler(uint32_t binding, const std::shared_ptr<Sampler> &sampler);
        BindGroupBuilder &addTexture(uint32_t binding, const std::shared_ptr<Texture> &texture);

        std::shared_ptr<BindGroup> build(const std::shared_ptr<BindGroupLayout> &layout, const std::shared_ptr<RenderContext> &context);

       private:
        std::string m_Label;
        std::vector<WGPUBindGroupEntry> m_Entries;
        std::vector<std::shared_ptr<Buffer>> m_Buffers;
        std::vector<std::shared_ptr<Sampler>> m_Samplers;
        std::vector<std::shared_ptr<Texture>> m_Textures;
    };
}  // namespace Yulduz