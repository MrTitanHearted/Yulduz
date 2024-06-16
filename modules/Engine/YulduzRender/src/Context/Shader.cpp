#include <YulduzRender/Context/Shader.hpp>
#include <YulduzRender/Context.hpp>

namespace Yulduz {
    Shader::Shader(const std::string &label, const WGPUShaderModule &shaderModule) {
        YZDEBUG("Initializig Shader: '{}'", label);

        m_Label = label;
        m_ShaderModule = shaderModule;
    }

    Shader::~Shader() {
        YZDEBUG("Releasing Shader: '{}'", m_Label);

        wgpuShaderModuleRelease(m_ShaderModule);
    }

    std::string Shader::getLabel() const {
        return m_Label;
    }

    WGPUShaderModule Shader::get() const {
        return m_ShaderModule;
    }

    ShaderBuilder::ShaderBuilder() {
        m_Label = "Yulduz Shader";
    }

    ShaderBuilder &ShaderBuilder::setLabel(const std::string &label) {
        m_Label = label;
        return *this;
    }

    std::shared_ptr<Shader> ShaderBuilder::buildWGSL(const std::shared_ptr<FileAsset> &asset, const std::shared_ptr<RenderContext> &context) {
        std::string source = asset->getContent();
        WGPUShaderModuleWGSLDescriptor wgslDescriptor{
            .chain = WGPUChainedStruct{.sType = WGPUSType_ShaderModuleWGSLDescriptor},
            .code = source.c_str(),
        };
        WGPUShaderModuleDescriptor descriptor{
            .nextInChain = &wgslDescriptor.chain,
            .label = m_Label.c_str(),
        };

        WGPUShaderModule shaderModule = wgpuDeviceCreateShaderModule(context->getDevice(), &descriptor);

        return std::make_shared<Shader>(m_Label, shaderModule);
    }

    std::shared_ptr<Shader> ShaderBuilder::buildGLSL(const std::shared_ptr<FileAsset> &asset, ShaderStage stages, const std::shared_ptr<RenderContext> &context) {
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

        WGPUShaderModule shaderModule = wgpuDeviceCreateShaderModule(context->getDevice(), &descriptor);

        return std::make_shared<Shader>(m_Label, shaderModule);
    }

    std::shared_ptr<Shader> ShaderBuilder::buildSPIRV(const std::shared_ptr<FileAsset> &asset, const std::shared_ptr<RenderContext> &context) {
        std::string source = asset->getContent();
        WGPUShaderModuleSPIRVDescriptor spirvDescriptor{
            .chain = WGPUChainedStruct{.sType = WGPUSType_ShaderModuleSPIRVDescriptor},
            .codeSize = static_cast<uint32_t>(source.size()),
            .code = (const uint32_t *)source.c_str(),
        };
        WGPUShaderModuleDescriptor descriptor{
            .nextInChain = &spirvDescriptor.chain,
            .label = m_Label.c_str(),
        };

        WGPUShaderModule shaderModule = wgpuDeviceCreateShaderModule(context->getDevice(), &descriptor);

        return std::make_shared<Shader>(m_Label, shaderModule);
    }
}  // namespace Yulduz