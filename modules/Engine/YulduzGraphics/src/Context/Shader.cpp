#include <YulduzGraphics/Context/Shader.hpp>
#include <YulduzGraphics/Context/GraphicsContext.hpp>


namespace Yulduz {
    Shader::Shader(const std::string &label, const WGPUShaderModule &handle) {
        assert(handle != nullptr && "Shader Module handle cannot be nullptr");

        m_ShaderModule = handle;
        m_Label = label;
    }

    Shader::~Shader() {
        if (m_ShaderModule)
            wgpuShaderModuleRelease(m_ShaderModule);
    }

    Shader::Shader(const Shader &other) {
        assert(other.m_ShaderModule != nullptr && "Shader Module handle cannot be nullptr");

        if (&other != this) {
            wgpuShaderModuleRelease(m_ShaderModule);

            m_ShaderModule = other.m_ShaderModule;
            m_Label = other.m_Label;

            wgpuShaderModuleReference(m_ShaderModule);
        }
    }

    Shader &Shader::operator=(const Shader &other) {
        assert(other.m_ShaderModule != nullptr && "Shader Module handle cannot be nullptr");

        if (&other != this) {
            wgpuShaderModuleRelease(m_ShaderModule);

            m_ShaderModule = other.m_ShaderModule;
            m_Label = other.m_Label;

            wgpuShaderModuleReference(m_ShaderModule);
        }

        return *this;
    }
    Shader::Shader(Shader &&other) {
        assert(other.m_ShaderModule != nullptr && "Shader Module handle cannot be nullptr");

        if (&other != this) {
            wgpuShaderModuleRelease(m_ShaderModule);

            m_ShaderModule = other.m_ShaderModule;
            m_Label = other.m_Label;

            other.m_ShaderModule = nullptr;
            other.m_Label = "";
        }
    }

    Shader &Shader::operator=(Shader &&other) {
        assert(other.m_ShaderModule != nullptr && "Shader Module handle cannot be nullptr");

        if (&other != this) {
            wgpuShaderModuleRelease(m_ShaderModule);

            m_ShaderModule = other.m_ShaderModule;
            m_Label = other.m_Label;

            other.m_ShaderModule = nullptr;
            other.m_Label = "";
        }

        return *this;
    }

    WGPUShaderModule Shader::get() const {
        assert(m_ShaderModule != nullptr && "Shader Module handle cannot be nullptr");

        return m_ShaderModule;
    }

    std::string Shader::getLabel() const {
        assert(m_ShaderModule != nullptr && "Shader Module handle cannot be nullptr");
        
        return m_Label;
    }

    ShaderBuilder::ShaderBuilder() {
        m_Label = "Yulduz Shader";
    }

    ShaderBuilder &ShaderBuilder::setLabel(const std::string &label) {
        m_Label = label;
        return *this;
    }

    Shader ShaderBuilder::buildWGSL(const std::shared_ptr<FileAsset> &asset, const GraphicsContext &context) {
        std::string source = asset->getContent();
        WGPUShaderModuleWGSLDescriptor wgslDescriptor{
            .chain = WGPUChainedStruct{.sType = WGPUSType_ShaderModuleWGSLDescriptor},
            .code = source.c_str(),
        };
        WGPUShaderModuleDescriptor descriptor{
            .nextInChain = &wgslDescriptor.chain,
            .label = m_Label.c_str(),
        };

        return Shader{m_Label, wgpuDeviceCreateShaderModule(context.getDevice(), &descriptor)};
    }

    Shader ShaderBuilder::buildGLSL(const std::shared_ptr<FileAsset> &asset, ShaderStage stages, const GraphicsContext &context) {
        std::string source = asset->getContent();
        WGPUShaderModuleGLSLDescriptor glslDescriptor{
            .chain = WGPUChainedStruct{.sType = static_cast<WGPUSType>(WGPUSType_ShaderModuleGLSLDescriptor)},
            .stage = static_cast<WGPUShaderStage>(stages),
            .code = source.c_str(),
        };
        WGPUShaderModuleDescriptor descriptor{
            .nextInChain = &glslDescriptor.chain,
            .label = m_Label.c_str(),
        };

        return Shader{m_Label, wgpuDeviceCreateShaderModule(context.getDevice(), &descriptor)};
    }

    Shader ShaderBuilder::buildSPIRV(const std::shared_ptr<FileAsset> &asset, const GraphicsContext &context) {
        std::string source = asset->getContent();
        WGPUShaderModuleSPIRVDescriptor spirvDescriptor{
            .chain = WGPUChainedStruct{.sType = WGPUSType_ShaderModuleSPIRVDescriptor},
            .codeSize = static_cast<std::uint32_t>(source.size()),
            .code = (const std::uint32_t *)source.c_str(),
        };
        WGPUShaderModuleDescriptor descriptor{
            .nextInChain = &spirvDescriptor.chain,
            .label = m_Label.c_str(),
        };

        return Shader{m_Label, wgpuDeviceCreateShaderModule(context.getDevice(), &descriptor)};
    }
}  // namespace Yulduz