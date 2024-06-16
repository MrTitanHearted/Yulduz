#pragma once

#include <Yulduz/Core.hpp>
#include <Yulduz/Assets.hpp>
#include <YulduzRender/Enums.hpp>

namespace Yulduz {
    class RenderContext;

    class Shader {
       public:
        Shader(const std::string &label, const WGPUShaderModule &shaderModule);
        ~Shader();

        std::string getLabel() const;
        WGPUShaderModule get() const;

       private:
        std::string m_Label;
        WGPUShaderModule m_ShaderModule;
    };

    class ShaderBuilder {
       public:
        ShaderBuilder();
        ~ShaderBuilder() = default;

        inline static ShaderBuilder New() { return ShaderBuilder(); }

        ShaderBuilder &setLabel(const std::string &label);

        std::shared_ptr<Shader> buildWGSL(const std::shared_ptr<FileAsset> &asset, const std::shared_ptr<RenderContext> &context);
        std::shared_ptr<Shader> buildGLSL(const std::shared_ptr<FileAsset> &asset, ShaderStage stages, const std::shared_ptr<RenderContext> &context);
        std::shared_ptr<Shader> buildSPIRV(const std::shared_ptr<FileAsset> &asset, const std::shared_ptr<RenderContext> &context);

       private:
        std::string m_Label;
    };
}  // namespace Yulduz