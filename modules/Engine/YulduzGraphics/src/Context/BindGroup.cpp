#include <YulduzGraphics/Context/BindGroup.hpp>
#include <YulduzGraphics/Context/GraphicsContext.hpp>

namespace Yulduz {
    BufferEntry::BufferEntry(ShaderStage stage) {
        m_ShaderStage = stage;
    }

    WGPUBindGroupLayoutEntry BufferEntry::getStorage(std::uint32_t binding) const {
        return WGPUBindGroupLayoutEntry{
            .binding = binding,
            .visibility = static_cast<WGPUShaderStageFlags>(m_ShaderStage),
            .buffer = WGPUBufferBindingLayout{.type = WGPUBufferBindingType_Storage},
        };
    }

    WGPUBindGroupLayoutEntry BufferEntry::getUniform(std::uint32_t binding) const {
        return WGPUBindGroupLayoutEntry{
            .binding = binding,
            .visibility = static_cast<WGPUShaderStageFlags>(m_ShaderStage),
            .buffer = WGPUBufferBindingLayout{.type = WGPUBufferBindingType_Uniform},
        };
    }

    SamplerEntry::SamplerEntry(ShaderStage stage) {
        m_ShaderStage = stage;
        m_SamplerBindingType = SamplerBindingType::Filtering;
    }

    SamplerEntry &SamplerEntry::setBindingType(SamplerBindingType type) {
        m_SamplerBindingType = type;
        return *this;
    }

    WGPUBindGroupLayoutEntry SamplerEntry::get(std::uint32_t binding) const {
        return WGPUBindGroupLayoutEntry{
            .binding = binding,
            .visibility = static_cast<WGPUShaderStageFlags>(m_ShaderStage),
            .sampler = WGPUSamplerBindingLayout{.type = static_cast<WGPUSamplerBindingType>(m_SamplerBindingType)},
        };
    }

    TextureEntry::TextureEntry(ShaderStage stage) {
        m_ShaderStage = stage;
        m_TextureSampleType = TextureSampleType::Float;
        m_IsMultisampled = false;
    }

    TextureEntry &TextureEntry::setTextureSampleType(TextureSampleType type) {
        m_TextureSampleType = type;
        return *this;
    }

    TextureEntry &TextureEntry::setIsMultisampled(bool isMultisampled) {
        m_IsMultisampled = isMultisampled;
        return *this;
    }

    WGPUBindGroupLayoutEntry TextureEntry::get2D(std::uint32_t binding) const {
        return WGPUBindGroupLayoutEntry{
            .binding = binding,
            .visibility = static_cast<WGPUShaderStageFlags>(m_ShaderStage),
            .texture = WGPUTextureBindingLayout{
                .sampleType = static_cast<WGPUTextureSampleType>(m_TextureSampleType),
                .viewDimension = WGPUTextureViewDimension_2D,
                .multisampled = m_IsMultisampled,
            },
        };
    }

    WGPUBindGroupLayoutEntry TextureEntry::get2DArray(std::uint32_t binding) const {
        return WGPUBindGroupLayoutEntry{
            .binding = binding,
            .visibility = static_cast<WGPUShaderStageFlags>(m_ShaderStage),
            .texture = WGPUTextureBindingLayout{
                .sampleType = static_cast<WGPUTextureSampleType>(m_TextureSampleType),
                .viewDimension = WGPUTextureViewDimension_2DArray,
                .multisampled = m_IsMultisampled,
            },
        };
    }

    WGPUBindGroupLayoutEntry TextureEntry::getCube(std::uint32_t binding) const {
        return WGPUBindGroupLayoutEntry{
            .binding = binding,
            .visibility = static_cast<WGPUShaderStageFlags>(m_ShaderStage),
            .texture = WGPUTextureBindingLayout{
                .sampleType = static_cast<WGPUTextureSampleType>(m_TextureSampleType),
                .viewDimension = WGPUTextureViewDimension_Cube,
                .multisampled = m_IsMultisampled,
            },
        };
    }

    WGPUBindGroupLayoutEntry TextureEntry::getCubeArray(std::uint32_t binding) const {
        return WGPUBindGroupLayoutEntry{
            .binding = binding,
            .visibility = static_cast<WGPUShaderStageFlags>(m_ShaderStage),
            .texture = WGPUTextureBindingLayout{
                .sampleType = static_cast<WGPUTextureSampleType>(m_TextureSampleType),
                .viewDimension = WGPUTextureViewDimension_CubeArray,
                .multisampled = m_IsMultisampled,
            },
        };
    }

    WGPUBindGroupLayoutEntry TextureEntry::get3D(std::uint32_t binding) const {
        return WGPUBindGroupLayoutEntry{
            .binding = binding,
            .visibility = static_cast<WGPUShaderStageFlags>(m_ShaderStage),
            .texture = WGPUTextureBindingLayout{
                .sampleType = static_cast<WGPUTextureSampleType>(m_TextureSampleType),
                .viewDimension = WGPUTextureViewDimension_3D,
                .multisampled = m_IsMultisampled,
            },
        };
    }

    StorageTextureEntry::StorageTextureEntry(TextureFormat format, ShaderStage stage) {
        m_ShaderStage = stage;
        m_TextureFormat = format;
        m_StorageTextureAccess = StorageTextureAccess::ReadWrite;
    }

    StorageTextureEntry &StorageTextureEntry::setStorageTextureAccess(StorageTextureAccess access) {
        m_StorageTextureAccess = access;
        return *this;
    }

    WGPUBindGroupLayoutEntry StorageTextureEntry::get2D(std::uint32_t binding) const {
        return WGPUBindGroupLayoutEntry{
            .binding = binding,
            .visibility = static_cast<WGPUShaderStageFlags>(m_ShaderStage),
            .storageTexture = WGPUStorageTextureBindingLayout{
                .access = static_cast<WGPUStorageTextureAccess>(m_StorageTextureAccess),
                .format = static_cast<WGPUTextureFormat>(m_TextureFormat),
                .viewDimension = WGPUTextureViewDimension_2D,
            },
        };
    }

    WGPUBindGroupLayoutEntry StorageTextureEntry::get2DArray(std::uint32_t binding) const {
        return WGPUBindGroupLayoutEntry{
            .binding = binding,
            .visibility = static_cast<WGPUShaderStageFlags>(m_ShaderStage),
            .storageTexture = WGPUStorageTextureBindingLayout{
                .access = static_cast<WGPUStorageTextureAccess>(m_StorageTextureAccess),
                .format = static_cast<WGPUTextureFormat>(m_TextureFormat),
                .viewDimension = WGPUTextureViewDimension_2DArray,
            },
        };
    }

    WGPUBindGroupLayoutEntry StorageTextureEntry::getCube(std::uint32_t binding) const {
        return WGPUBindGroupLayoutEntry{
            .binding = binding,
            .visibility = static_cast<WGPUShaderStageFlags>(m_ShaderStage),
            .storageTexture = WGPUStorageTextureBindingLayout{
                .access = static_cast<WGPUStorageTextureAccess>(m_StorageTextureAccess),
                .format = static_cast<WGPUTextureFormat>(m_TextureFormat),
                .viewDimension = WGPUTextureViewDimension_Cube,
            },
        };
    }

    WGPUBindGroupLayoutEntry StorageTextureEntry::getCubeArray(std::uint32_t binding) const {
        return WGPUBindGroupLayoutEntry{
            .binding = binding,
            .visibility = static_cast<WGPUShaderStageFlags>(m_ShaderStage),
            .storageTexture = WGPUStorageTextureBindingLayout{
                .access = static_cast<WGPUStorageTextureAccess>(m_StorageTextureAccess),
                .format = static_cast<WGPUTextureFormat>(m_TextureFormat),
                .viewDimension = WGPUTextureViewDimension_CubeArray,
            },
        };
    }

    WGPUBindGroupLayoutEntry StorageTextureEntry::get3D(std::uint32_t binding) const {
        return WGPUBindGroupLayoutEntry{
            .binding = binding,
            .visibility = static_cast<WGPUShaderStageFlags>(m_ShaderStage),
            .storageTexture = WGPUStorageTextureBindingLayout{
                .access = static_cast<WGPUStorageTextureAccess>(m_StorageTextureAccess),
                .format = static_cast<WGPUTextureFormat>(m_TextureFormat),
                .viewDimension = WGPUTextureViewDimension_3D,
            },
        };
    }

    BindGroupLayout::BindGroupLayout(const WGPUBindGroupLayout &handle) : m_BindGroupLayout{handle} {
        assert(handle != nullptr && "BindGroupLayout handle cannot be nullptr");
    }

    BindGroupLayout::BindGroupLayout() {
        m_BindGroupLayout = nullptr;
    }

    BindGroupLayout::~BindGroupLayout() {
        if (m_BindGroupLayout)
            wgpuBindGroupLayoutRelease(m_BindGroupLayout);
    }

    BindGroupLayout::BindGroupLayout(const BindGroupLayout &other) : m_BindGroupLayout{other.m_BindGroupLayout} {
        assert(m_BindGroupLayout != nullptr && "BindGroupLayout handle cannot be nullptr");

        wgpuBindGroupLayoutReference(m_BindGroupLayout);
    }

    BindGroupLayout::BindGroupLayout(BindGroupLayout &&other) : m_BindGroupLayout{other.m_BindGroupLayout} {
        assert(other.m_BindGroupLayout != nullptr && "BindGroupLayout handle cannot be nullptr");

        other.m_BindGroupLayout = nullptr;
    }

    BindGroupLayout &BindGroupLayout::operator=(const BindGroupLayout &other) {
        assert(other.m_BindGroupLayout != nullptr && "BindGroupLayout handle cannot be nullptr");

        if (&other != this) {
            if (m_BindGroupLayout) wgpuBindGroupLayoutRelease(m_BindGroupLayout);

            m_BindGroupLayout = other.m_BindGroupLayout;

            wgpuBindGroupLayoutReference(m_BindGroupLayout);
        }

        return *this;
    }

    BindGroupLayout &BindGroupLayout::operator=(BindGroupLayout &&other) {
        assert(other.m_BindGroupLayout != nullptr && "BindGroupLayout handle cannot be nullptr");

        if (&other != this) {
            if (m_BindGroupLayout) wgpuBindGroupLayoutRelease(m_BindGroupLayout);

            m_BindGroupLayout = other.m_BindGroupLayout;

            other.m_BindGroupLayout = nullptr;
        }

        return *this;
    }

    WGPUBindGroupLayout BindGroupLayout::get() const {
        assert(m_BindGroupLayout != nullptr && "BindGroupLayout handle cannot be nullptr");

        return m_BindGroupLayout;
    }

    BindGroup::BindGroup(const std::string &label,
                         const WGPUBindGroup &handle,
                         const BindGroupLayout &layout,
                         const std::vector<Buffer> &buffers,
                         const std::vector<Sampler> &samplers,
                         const std::vector<Texture> &textures)
        : m_BindGroup{handle},
          m_Label{label},
          m_LayoutReference{layout},
          m_BufferReferences{buffers},
          m_SamplerReferences{samplers},
          m_TextureReferences{textures} {
        assert(handle != nullptr && "BindGroup handle cannot be nullptr");
    }

    BindGroup::BindGroup() {
        m_BindGroup = nullptr;
        m_Label = "";
    }

    BindGroup::~BindGroup() {
        if (m_BindGroup)
            wgpuBindGroupRelease(m_BindGroup);
    }

    BindGroup::BindGroup(const BindGroup &other)
        : m_BindGroup{other.m_BindGroup},
          m_Label{other.m_Label},
          m_LayoutReference{other.m_LayoutReference},
          m_BufferReferences{other.m_BufferReferences},
          m_SamplerReferences{other.m_SamplerReferences},
          m_TextureReferences{other.m_TextureReferences} {
        assert(other.m_BindGroup != nullptr && "BindGroup handle cannot be nullptr");

        wgpuBindGroupReference(m_BindGroup);
    }

    BindGroup::BindGroup(BindGroup &&other)
        : m_BindGroup{other.m_BindGroup},
          m_Label{other.m_Label},
          m_LayoutReference{other.m_LayoutReference},
          m_BufferReferences{other.m_BufferReferences},
          m_SamplerReferences{other.m_SamplerReferences},
          m_TextureReferences{other.m_TextureReferences} {
        assert(other.m_BindGroup != nullptr && "BindGroup handle cannot be nullptr");

        other.m_BindGroup = nullptr;
        other.m_Label = "";
        other.m_LayoutReference = {};
        other.m_BufferReferences.clear();
        other.m_SamplerReferences.clear();
        other.m_TextureReferences.clear();
    }

    BindGroup &BindGroup::operator=(const BindGroup &other) {
        assert(other.m_BindGroup != nullptr && "BindGroup handle cannot be nullptr");

        if (&other != this) {
            if (m_BindGroup) wgpuBindGroupRelease(m_BindGroup);

            m_BindGroup = other.m_BindGroup;
            m_Label = other.m_Label;
            m_LayoutReference = other.m_LayoutReference;
            m_BufferReferences = other.m_BufferReferences;
            m_SamplerReferences = other.m_SamplerReferences;
            m_TextureReferences = other.m_TextureReferences;

            wgpuBindGroupReference(m_BindGroup);
        }

        return *this;
    }

    BindGroup &BindGroup::operator=(BindGroup &&other) {
        assert(other.m_BindGroup != nullptr && "BindGroup handle cannot be nullptr");

        if (&other != this) {
            if (m_BindGroup) wgpuBindGroupRelease(m_BindGroup);

            m_BindGroup = other.m_BindGroup;
            m_Label = other.m_Label;
            m_LayoutReference = std::move(other.m_LayoutReference);
            m_BufferReferences = other.m_BufferReferences;
            m_SamplerReferences = other.m_SamplerReferences;
            m_TextureReferences = other.m_TextureReferences;

            other.m_BindGroup = nullptr;
            other.m_Label = "";
            other.m_BufferReferences.clear();
            other.m_SamplerReferences.clear();
            other.m_TextureReferences.clear();
        }

        return *this;
    }

    std::string BindGroup::getLabel() const {
        assert(m_BindGroup != nullptr && "BindGroup handle cannot be nullptr");

        return m_Label;
    }
    WGPUBindGroup BindGroup::get() const {
        assert(m_BindGroup != nullptr && "BindGroup handle cannot be nullptr");

        return m_BindGroup;
    }
    const BindGroupLayout &BindGroup::getLayoutRef() const {
        assert(m_BindGroup != nullptr && "BindGroup handle cannot be nullptr");

        return m_LayoutReference;
    }

    BindGroupLayoutBuilder::BindGroupLayoutBuilder() {
        m_Label = "Yulduz BindGroup Layout";
    }

    BindGroupLayoutBuilder &BindGroupLayoutBuilder::setLabel(const std::string &label) {
        m_Label = label;
        return *this;
    }

    BindGroupLayoutBuilder &BindGroupLayoutBuilder::addStorageBuffer(std::uint32_t binding, const BufferEntry &entry) {
        m_Entries.emplace_back(entry.getStorage(binding));
        return *this;
    }

    BindGroupLayoutBuilder &BindGroupLayoutBuilder::addUniformBuffer(std::uint32_t binding, const BufferEntry &entry) {
        m_Entries.emplace_back(entry.getUniform(binding));
        return *this;
    }

    BindGroupLayoutBuilder &BindGroupLayoutBuilder::addSampler(std::uint32_t binding, const SamplerEntry &entry) {
        m_Entries.emplace_back(entry.get(binding));
        return *this;
    }

    BindGroupLayoutBuilder &BindGroupLayoutBuilder::addTexture2D(std::uint32_t binding, const TextureEntry &entry) {
        m_Entries.emplace_back(entry.get2D(binding));
        return *this;
    }

    BindGroupLayoutBuilder &BindGroupLayoutBuilder::addTexture2DArray(std::uint32_t binding, const TextureEntry &entry) {
        m_Entries.emplace_back(entry.get2DArray(binding));
        return *this;
    }

    BindGroupLayoutBuilder &BindGroupLayoutBuilder::addTextureCube(std::uint32_t binding, const TextureEntry &entry) {
        m_Entries.emplace_back(entry.getCube(binding));
        return *this;
    }

    BindGroupLayoutBuilder &BindGroupLayoutBuilder::addTextureCubeArray(std::uint32_t binding, const TextureEntry &entry) {
        m_Entries.emplace_back(entry.getCubeArray(binding));
        return *this;
    }

    BindGroupLayoutBuilder &BindGroupLayoutBuilder::addTexture3D(std::uint32_t binding, const TextureEntry &entry) {
        m_Entries.emplace_back(entry.get3D(binding));
        return *this;
    }

    BindGroupLayoutBuilder &BindGroupLayoutBuilder::addStorageTexture2D(std::uint32_t binding, const StorageTextureEntry &entry) {
        m_Entries.emplace_back(entry.get2D(binding));
        return *this;
    }

    BindGroupLayoutBuilder &BindGroupLayoutBuilder::addStorageTexture2DArray(std::uint32_t binding, const StorageTextureEntry &entry) {
        m_Entries.emplace_back(entry.get2DArray(binding));
        return *this;
    }

    BindGroupLayoutBuilder &BindGroupLayoutBuilder::addStorageTextureCube(std::uint32_t binding, const StorageTextureEntry &entry) {
        m_Entries.emplace_back(entry.getCube(binding));
        return *this;
    }

    BindGroupLayoutBuilder &BindGroupLayoutBuilder::addStorageTextureCubeArray(std::uint32_t binding, const StorageTextureEntry &entry) {
        m_Entries.emplace_back(entry.getCubeArray(binding));
        return *this;
    }

    BindGroupLayoutBuilder &BindGroupLayoutBuilder::addStorageTexture3D(std::uint32_t binding, const StorageTextureEntry &entry) {
        m_Entries.emplace_back(entry.get3D(binding));
        return *this;
    }

    BindGroupLayout BindGroupLayoutBuilder::build(const GraphicsContext &context) {
        WGPUBindGroupLayoutDescriptor descriptor{
            .label = m_Label.c_str(),
            .entryCount = m_Entries.size(),
            .entries = m_Entries.data(),
        };

        return BindGroupLayout{wgpuDeviceCreateBindGroupLayout(context.getDevice(), &descriptor)};
    }

    BindGroupBuilder::BindGroupBuilder() {
        m_Label = "Yulduz BindGroup";
    }

    BindGroupBuilder &BindGroupBuilder::setLabel(const std::string &label) {
        m_Label = label;
        return *this;
    }

    BindGroupBuilder &BindGroupBuilder::addStorageBuffer(std::uint32_t binding, const StorageBuffer &buffer) {
        m_Entries.emplace_back(WGPUBindGroupEntry{.binding = binding, .buffer = buffer.get(), .size = buffer.getSize()});
        m_Buffers.emplace_back(buffer);
        return *this;
    }

    BindGroupBuilder &BindGroupBuilder::addUniformBuffer(std::uint32_t binding, const UniformBuffer &buffer) {
        m_Entries.emplace_back(WGPUBindGroupEntry{.binding = binding, .buffer = buffer.get(), .size = buffer.getSize()});
        m_Buffers.emplace_back(buffer);
        return *this;
    }

    BindGroupBuilder &BindGroupBuilder::addSampler(std::uint32_t binding, const Sampler &sampler) {
        m_Entries.emplace_back(WGPUBindGroupEntry{.binding = binding, .sampler = sampler.get()});
        m_Samplers.emplace_back(sampler);
        return *this;
    }

    BindGroupBuilder &BindGroupBuilder::addTexture(std::uint32_t binding, const Texture &texture) {
        m_Entries.emplace_back(WGPUBindGroupEntry{.binding = binding, .textureView = texture.getDefaultView()});
        m_Textures.emplace_back(texture);
        return *this;
    }

    BindGroupBuilder &BindGroupBuilder::addTexture(std::uint32_t binding, const TextureView &view) {
        m_Entries.emplace_back(WGPUBindGroupEntry{.binding = binding, .textureView = view.get()});
        m_Textures.emplace_back(view.getTextureRef());
        return *this;
    }

    BindGroup BindGroupBuilder::build(const BindGroupLayout &layout, const GraphicsContext &context) {
        WGPUBindGroupDescriptor descriptor{
            .label = m_Label.c_str(),
            .layout = layout.get(),
            .entryCount = m_Entries.size(),
            .entries = m_Entries.data(),
        };

        return BindGroup{m_Label, wgpuDeviceCreateBindGroup(context.getDevice(), &descriptor), layout, m_Buffers, m_Samplers, m_Textures};
    }
}  // namespace Yulduz
