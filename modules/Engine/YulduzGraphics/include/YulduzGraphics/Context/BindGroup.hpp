#pragma once

#include <Yulduz/Core.hpp>
#include <YulduzGraphics/Enums.hpp>
#include <YulduzGraphics/Context/Buffer.hpp>
#include <YulduzGraphics/Context/Sampler.hpp>
#include <YulduzGraphics/Context/Texture.hpp>

namespace Yulduz {
    class GraphicsContext;

    class BufferEntry {
       public:
        BufferEntry(ShaderStage stage);
        ~BufferEntry() = default;

        inline static BufferEntry New(ShaderStage stage) { return BufferEntry{stage}; }

        WGPUBindGroupLayoutEntry getStorage(std::uint32_t binding) const;
        WGPUBindGroupLayoutEntry getUniform(std::uint32_t binding) const;

       private:
        ShaderStage m_ShaderStage;
    };

    class SamplerEntry {
       public:
        SamplerEntry(ShaderStage stage);
        ~SamplerEntry() = default;

        inline static SamplerEntry New(ShaderStage stage) { return SamplerEntry{stage}; }

        SamplerEntry &setBindingType(SamplerBindingType type);

        WGPUBindGroupLayoutEntry get(std::uint32_t binding) const;

       private:
        ShaderStage m_ShaderStage;
        SamplerBindingType m_SamplerBindingType;
    };

    class TextureEntry {
       public:
        TextureEntry(ShaderStage stage);
        ~TextureEntry() = default;

        inline static TextureEntry New(ShaderStage stage) { return TextureEntry{stage}; }

        TextureEntry &setTextureSampleType(TextureSampleType type);
        TextureEntry &setIsMultisampled(bool isMultisampled);

        WGPUBindGroupLayoutEntry get2D(std::uint32_t binding) const;
        WGPUBindGroupLayoutEntry get2DArray(std::uint32_t binding) const;
        WGPUBindGroupLayoutEntry getCube(std::uint32_t binding) const;
        WGPUBindGroupLayoutEntry getCubeArray(std::uint32_t binding) const;
        WGPUBindGroupLayoutEntry get3D(std::uint32_t binding) const;

       private:
        ShaderStage m_ShaderStage;
        TextureSampleType m_TextureSampleType;
        bool m_IsMultisampled;
    };

    class StorageTextureEntry {
       public:
        StorageTextureEntry(TextureFormat format, ShaderStage stage);
        ~StorageTextureEntry() = default;

        inline static StorageTextureEntry New(TextureFormat format, ShaderStage stage) { return StorageTextureEntry{format, stage}; }

        StorageTextureEntry &setStorageTextureAccess(StorageTextureAccess access);

        WGPUBindGroupLayoutEntry get2D(std::uint32_t binding) const;
        WGPUBindGroupLayoutEntry get2DArray(std::uint32_t binding) const;
        WGPUBindGroupLayoutEntry getCube(std::uint32_t binding) const;
        WGPUBindGroupLayoutEntry getCubeArray(std::uint32_t binding) const;
        WGPUBindGroupLayoutEntry get3D(std::uint32_t binding) const;

       private:
        ShaderStage m_ShaderStage;
        StorageTextureAccess m_StorageTextureAccess;
        TextureFormat m_TextureFormat;
    };

    class BindGroupLayout {
       public:
        BindGroupLayout(const WGPUBindGroupLayout &handle);

        BindGroupLayout();
        ~BindGroupLayout();

        BindGroupLayout(const BindGroupLayout &other);
        BindGroupLayout(BindGroupLayout &&other);

        BindGroupLayout &operator=(const BindGroupLayout &other);
        BindGroupLayout &operator=(BindGroupLayout &&other);

        WGPUBindGroupLayout get() const;

       private:
        WGPUBindGroupLayout m_BindGroupLayout;
    };

    class BindGroup {
       public:
        BindGroup(const std::string &label,
                  const WGPUBindGroup &handle,
                  const BindGroupLayout &layout,
                  const std::vector<Buffer> &buffers,
                  const std::vector<Sampler> &samplers,
                  const std::vector<Texture> &textures);

        BindGroup();
        ~BindGroup();

        BindGroup(const BindGroup &other);
        BindGroup(BindGroup &&other);

        BindGroup &operator=(const BindGroup &other);
        BindGroup &operator=(BindGroup &&other);

        std::string getLabel() const;
        WGPUBindGroup get() const;
        const BindGroupLayout &getLayoutRef() const;

       private:
        std::string m_Label;
        WGPUBindGroup m_BindGroup;

        BindGroupLayout m_LayoutReference;
        std::vector<Buffer> m_BufferReferences;
        std::vector<Sampler> m_SamplerReferences;
        std::vector<Texture> m_TextureReferences;
    };

    class BindGroupLayoutBuilder {
       public:
        BindGroupLayoutBuilder();
        ~BindGroupLayoutBuilder() = default;

        inline static BindGroupLayoutBuilder New() { return BindGroupLayoutBuilder(); }

        BindGroupLayoutBuilder &setLabel(const std::string &label);
        BindGroupLayoutBuilder &addStorageBuffer(std::uint32_t binding, const BufferEntry &entry);
        BindGroupLayoutBuilder &addUniformBuffer(std::uint32_t binding, const BufferEntry &entry);
        BindGroupLayoutBuilder &addSampler(std::uint32_t binding, const SamplerEntry &entry);
        BindGroupLayoutBuilder &addTexture2D(std::uint32_t binding, const TextureEntry &entry);
        BindGroupLayoutBuilder &addTexture2DArray(std::uint32_t binding, const TextureEntry &entry);
        BindGroupLayoutBuilder &addTextureCube(std::uint32_t binding, const TextureEntry &entry);
        BindGroupLayoutBuilder &addTextureCubeArray(std::uint32_t binding, const TextureEntry &entry);
        BindGroupLayoutBuilder &addTexture3D(std::uint32_t binding, const TextureEntry &entry);
        BindGroupLayoutBuilder &addStorageTexture2D(std::uint32_t binding, const StorageTextureEntry &entry);
        BindGroupLayoutBuilder &addStorageTexture2DArray(std::uint32_t binding, const StorageTextureEntry &entry);
        BindGroupLayoutBuilder &addStorageTextureCube(std::uint32_t binding, const StorageTextureEntry &entry);
        BindGroupLayoutBuilder &addStorageTextureCubeArray(std::uint32_t binding, const StorageTextureEntry &entry);
        BindGroupLayoutBuilder &addStorageTexture3D(std::uint32_t binding, const StorageTextureEntry &entry);

        BindGroupLayout build(const GraphicsContext &context);

        inline BindGroupLayoutBuilder &addStorageBuffer(std::uint32_t binding) {
            return addStorageBuffer(binding, BufferEntry{ShaderStage::Fragment | ShaderStage::Vertex | ShaderStage::Compute});
        }
        inline BindGroupLayoutBuilder &addUniformBuffer(std::uint32_t binding) {
            return addUniformBuffer(binding, BufferEntry{ShaderStage::Fragment | ShaderStage::Vertex | ShaderStage::Compute});
        }
        inline BindGroupLayoutBuilder &addSampler(std::uint32_t binding) {
            return addSampler(binding, SamplerEntry{ShaderStage::Fragment});
        }
        inline BindGroupLayoutBuilder &addTexture2D(std::uint32_t binding) {
            return addTexture2D(binding, TextureEntry{ShaderStage::Fragment});
        }
        inline BindGroupLayoutBuilder &addTexture2DArray(std::uint32_t binding) {
            return addTexture2DArray(binding, TextureEntry{ShaderStage::Fragment});
        }
        inline BindGroupLayoutBuilder &addTextureCube(std::uint32_t binding) {
            return addTextureCube(binding, TextureEntry{ShaderStage::Fragment});
        }
        inline BindGroupLayoutBuilder &addTextureCubeArray(std::uint32_t binding) {
            return addTextureCubeArray(binding, TextureEntry{ShaderStage::Fragment});
        }
        inline BindGroupLayoutBuilder &addTexture3D(std::uint32_t binding) {
            return addTexture3D(binding, TextureEntry{ShaderStage::Fragment});
        }
        inline BindGroupLayoutBuilder &addStorageTexture2D(std::uint32_t binding, TextureFormat format) {
            return addStorageTexture2D(binding, StorageTextureEntry{format, ShaderStage::Fragment | ShaderStage::Compute});
        }
        inline BindGroupLayoutBuilder &addStorageTexture2DArray(std::uint32_t binding, TextureFormat format) {
            return addStorageTexture2DArray(binding, StorageTextureEntry{format, ShaderStage::Fragment | ShaderStage::Compute});
        }
        inline BindGroupLayoutBuilder &addStorageTextureCube(std::uint32_t binding, TextureFormat format) {
            return addStorageTextureCube(binding, StorageTextureEntry{format, ShaderStage::Fragment | ShaderStage::Compute});
        }
        inline BindGroupLayoutBuilder &addStorageTextureCubeArray(std::uint32_t binding, TextureFormat format) {
            return addStorageTextureCubeArray(binding, StorageTextureEntry{format, ShaderStage::Fragment | ShaderStage::Compute});
        }
        inline BindGroupLayoutBuilder &addStorageTexture3D(std::uint32_t binding, TextureFormat format) {
            return addStorageTexture3D(binding, StorageTextureEntry{format, ShaderStage::Fragment | ShaderStage::Compute});
        }

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
        BindGroupBuilder &addStorageBuffer(std::uint32_t binding, const StorageBuffer &buffer);
        BindGroupBuilder &addUniformBuffer(std::uint32_t binding, const UniformBuffer &buffer);
        BindGroupBuilder &addSampler(std::uint32_t binding, const Sampler &sampler);
        BindGroupBuilder &addTexture(std::uint32_t binding, const Texture &texture);
        BindGroupBuilder &addTexture(std::uint32_t binding, const TextureView &view);

        BindGroup build(const BindGroupLayout &layout, const GraphicsContext &context);

       private:
        std::string m_Label;
        std::vector<WGPUBindGroupEntry> m_Entries;

        std::vector<Buffer> m_Buffers;
        std::vector<Sampler> m_Samplers;
        std::vector<Texture> m_Textures;
    };
}  // namespace Yulduz
