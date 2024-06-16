#include <YulduzRender/Context/BindGroup.hpp>
#include <YulduzRender/Context.hpp>

namespace Yulduz {
    BufferEntry::BufferEntry(ShaderStage stage) {
        m_ShaderStage = stage;
    }

    BufferEntry &BufferEntry::setShaderStage(ShaderStage stage) {
        m_ShaderStage = stage;
        return *this;
    }

    WGPUBindGroupLayoutEntry BufferEntry::getStorage(uint32_t binding) const {
        return WGPUBindGroupLayoutEntry{
            .binding = binding,
            .visibility = static_cast<WGPUShaderStageFlags>(m_ShaderStage),
            .buffer = {
                .type = WGPUBufferBindingType_Storage,
            },
        };
    }

    WGPUBindGroupLayoutEntry BufferEntry::getUniform(uint32_t binding) const {
        return WGPUBindGroupLayoutEntry{
            .binding = binding,
            .visibility = static_cast<WGPUShaderStageFlags>(m_ShaderStage),
            .buffer = {
                .type = WGPUBufferBindingType_Uniform,
            },
        };
    }

    SamplerEntry::SamplerEntry(ShaderStage stage) {
        m_ShaderStage = stage;
        m_SamplerBindingType = SamplerBindingType::Filtering;
    }

    SamplerEntry &SamplerEntry::setShaderStage(ShaderStage stage) {
        m_ShaderStage = stage;
        return *this;
    }

    SamplerEntry &SamplerEntry::setBindingType(SamplerBindingType type) {
        m_SamplerBindingType = type;
        return *this;
    }

    WGPUBindGroupLayoutEntry SamplerEntry::get(uint32_t binding) const {
        return WGPUBindGroupLayoutEntry{
            .binding = binding,
            .visibility = static_cast<WGPUShaderStageFlags>(m_ShaderStage),
            .sampler = {
                .type = static_cast<WGPUSamplerBindingType>(m_SamplerBindingType),
            },
        };
    }

    TextureEntry::TextureEntry(ShaderStage stage) {
        m_ShaderStage = stage;
        m_TextureSampleType = TextureSampleType::Float;
        m_IsMultisampled = false;
    }

    TextureEntry &TextureEntry::setShaderStage(ShaderStage stage) {
        m_ShaderStage = stage;
        return *this;
    }

    TextureEntry &TextureEntry::setTextureSampleType(TextureSampleType type) {
        m_TextureSampleType = type;
        return *this;
    }

    TextureEntry &TextureEntry::setIsMultisampled(bool isMultisampled) {
        m_IsMultisampled = isMultisampled;
        return *this;
    }

    WGPUBindGroupLayoutEntry TextureEntry::get2d(uint32_t binding) const {
        return WGPUBindGroupLayoutEntry{
            .binding = binding,
            .visibility = static_cast<WGPUShaderStageFlags>(m_ShaderStage),
            .texture = {
                .sampleType = static_cast<WGPUTextureSampleType>(m_TextureSampleType),
                .viewDimension = WGPUTextureViewDimension_2D,
                .multisampled = m_IsMultisampled,
            },
        };
    }

    WGPUBindGroupLayoutEntry TextureEntry::get3d(uint32_t binding) const {
        return WGPUBindGroupLayoutEntry{
            .binding = binding,
            .visibility = static_cast<WGPUShaderStageFlags>(m_ShaderStage),
            .texture = {
                .sampleType = static_cast<WGPUTextureSampleType>(m_TextureSampleType),
                .viewDimension = WGPUTextureViewDimension_3D,
                .multisampled = m_IsMultisampled,
            },
        };
    }

    StorageTextureEntry::StorageTextureEntry(ShaderStage stage, TextureFormat format) {
        m_ShaderStage = stage;
        m_StorageTextureAccess = StorageTextureAccess::ReadWrite;
        m_TextureFormat = format;
    }

    StorageTextureEntry &StorageTextureEntry::setShaderStage(ShaderStage stage) {
        m_ShaderStage = stage;
        return *this;
    }

    StorageTextureEntry &StorageTextureEntry::setStorageTextureAccess(StorageTextureAccess access) {
        m_StorageTextureAccess = access;
        return *this;
    }

    StorageTextureEntry &StorageTextureEntry::setTextureFormat(TextureFormat format) {
        m_TextureFormat = format;
        return *this;
    }

    WGPUBindGroupLayoutEntry StorageTextureEntry::get2d(uint32_t binding) const {
        return WGPUBindGroupLayoutEntry{
            .binding = binding,
            .visibility = static_cast<WGPUShaderStageFlags>(m_ShaderStage),
            .storageTexture = {
                .access = static_cast<WGPUStorageTextureAccess>(m_StorageTextureAccess),
                .format = static_cast<WGPUTextureFormat>(m_TextureFormat),
                .viewDimension = WGPUTextureViewDimension_2D,
            },
        };
    }

    WGPUBindGroupLayoutEntry StorageTextureEntry::get3d(uint32_t binding) const {
        return WGPUBindGroupLayoutEntry{
            .binding = binding,
            .visibility = static_cast<WGPUShaderStageFlags>(m_ShaderStage),
            .storageTexture = {
                .access = static_cast<WGPUStorageTextureAccess>(m_StorageTextureAccess),
                .format = static_cast<WGPUTextureFormat>(m_TextureFormat),
                .viewDimension = WGPUTextureViewDimension_3D,
            },
        };
    }

    BindGroupLayout::BindGroupLayout(const std::string &label, const WGPUBindGroupLayout &bindGroupLayout) {
        YZDEBUG("Initializing Bind Group Layout: '{}'", label);

        m_Label = label;
        m_BindGroupLayout = bindGroupLayout;
    }

    BindGroupLayout::~BindGroupLayout() {
        YZDEBUG("Releasing Bind Group Layout: '{}'", m_Label);

        wgpuBindGroupLayoutRelease(m_BindGroupLayout);
    }

    std::string BindGroupLayout::getLabel() const {
        return m_Label;
    }

    WGPUBindGroupLayout BindGroupLayout::get() const {
        return m_BindGroupLayout;
    }

    BindGroup::BindGroup(const std::string &label,
                         const WGPUBindGroup &bindGroup,
                         const std::shared_ptr<BindGroupLayout> &bindGroupLayout,
                         const std::vector<std::shared_ptr<Buffer>> &buffers,
                         const std::vector<std::shared_ptr<Sampler>> &samplers,
                         const std::vector<std::shared_ptr<Texture>> &textures) {
        YZDEBUG("Initializing Bind Group: '{}'", label);

        m_Label = label;
        m_BindGroup = bindGroup;
        m_BindGroupLayout = bindGroupLayout;
        m_Buffers = buffers;
        m_Samplers = samplers;
        m_Textures = textures;
    }

    BindGroup::~BindGroup() {
        YZDEBUG("Releasing Bind Group: '{}'", m_Label);

        wgpuBindGroupRelease(m_BindGroup);
    }

    std::string BindGroup::getLabel() const {
        return m_Label;
    }

    WGPUBindGroup BindGroup::get() const {
        return m_BindGroup;
    }

    std::shared_ptr<BindGroupLayout> BindGroup::getLayout() const {
        return m_BindGroupLayout;
    }

    BindGroupLayoutBuilder::BindGroupLayoutBuilder() {
        m_Label = "Yulduz Bind Group Layout";
    }

    BindGroupLayoutBuilder &BindGroupLayoutBuilder::setLabel(const std::string &label) {
        m_Label = label;
        return *this;
    }

    BindGroupLayoutBuilder &BindGroupLayoutBuilder::addStorageBuffer(uint32_t binding, const BufferEntry &entry) {
        m_Entries.emplace_back(entry.getStorage(binding));
        return *this;
    }

    BindGroupLayoutBuilder &BindGroupLayoutBuilder::addUniformBuffer(uint32_t binding, const BufferEntry &entry) {
        m_Entries.emplace_back(entry.getUniform(binding));
        return *this;
    }

    BindGroupLayoutBuilder &BindGroupLayoutBuilder::addSampler(uint32_t binding, const SamplerEntry &entry) {
        m_Entries.emplace_back(entry.get(binding));
        return *this;
    }

    BindGroupLayoutBuilder &BindGroupLayoutBuilder::addTexture2D(uint32_t binding, const TextureEntry &entry) {
        m_Entries.emplace_back(entry.get2d(binding));
        return *this;
    }

    BindGroupLayoutBuilder &BindGroupLayoutBuilder::addTexture3D(uint32_t binding, const TextureEntry &entry) {
        m_Entries.emplace_back(entry.get3d(binding));
        return *this;
    }

    BindGroupLayoutBuilder &BindGroupLayoutBuilder::addStorageTexture2D(uint32_t binding, const StorageTextureEntry &entry) {
        m_Entries.emplace_back(entry.get2d(binding));
        return *this;
    }

    BindGroupLayoutBuilder &BindGroupLayoutBuilder::addStorageTexture3D(uint32_t binding, const StorageTextureEntry &entry) {
        m_Entries.emplace_back(entry.get3d(binding));
        return *this;
    }

    std::shared_ptr<BindGroupLayout> BindGroupLayoutBuilder::build(const std::shared_ptr<RenderContext> &context) {
        WGPUBindGroupLayoutDescriptor descriptor{
            .label = m_Label.c_str(),
            .entryCount = m_Entries.size(),
            .entries = m_Entries.data(),
        };

        WGPUBindGroupLayout bindGroupLayout = wgpuDeviceCreateBindGroupLayout(context->getDevice(), &descriptor);

        return std::make_shared<BindGroupLayout>(m_Label, bindGroupLayout);
    }

    BindGroupBuilder::BindGroupBuilder() {
        m_Label = "Yulduz Bind Group";
    }

    BindGroupBuilder &BindGroupBuilder::setLabel(const std::string &label) {
        m_Label = label;
        return *this;
    }

    BindGroupBuilder &BindGroupBuilder::addStorageBuffer(uint32_t binding, const std::shared_ptr<StorageBuffer> &buffer) {
        m_Entries.emplace_back(WGPUBindGroupEntry{.binding = binding, .buffer = buffer->get(), .size = buffer->getSize()});
        m_Buffers.emplace_back(buffer);
        return *this;
    }

    BindGroupBuilder &BindGroupBuilder::addUniformBuffer(uint32_t binding, const std::shared_ptr<UniformBuffer> &buffer) {
        m_Entries.emplace_back(WGPUBindGroupEntry{.binding = binding, .buffer = buffer->get(), .size = buffer->getSize()});
        m_Buffers.emplace_back(buffer);
        return *this;
    }

    BindGroupBuilder &BindGroupBuilder::addSampler(uint32_t binding, const std::shared_ptr<Sampler> &sampler) {
        m_Entries.emplace_back(WGPUBindGroupEntry{.binding = binding, .sampler = sampler->get()});
        m_Samplers.emplace_back(sampler);
        return *this;
    }

    BindGroupBuilder &BindGroupBuilder::addTexture(uint32_t binding, const std::shared_ptr<Texture> &texture) {
        m_Entries.emplace_back(WGPUBindGroupEntry{.binding = binding, .textureView = texture->getView()});
        m_Textures.emplace_back(texture);
        return *this;
    }

    std::shared_ptr<BindGroup> BindGroupBuilder::build(const std::shared_ptr<BindGroupLayout> &layout, const std::shared_ptr<RenderContext> &context) {
        WGPUBindGroupDescriptor descriptor{
            .label = m_Label.c_str(),
            .layout = layout->get(),
            .entryCount = m_Entries.size(),
            .entries = m_Entries.data(),
        };

        WGPUBindGroup bindGroup = wgpuDeviceCreateBindGroup(context->getDevice(), &descriptor);

        return std::make_shared<BindGroup>(m_Label, bindGroup, layout, m_Buffers, m_Samplers, m_Textures);
    }
}  // namespace Yulduz