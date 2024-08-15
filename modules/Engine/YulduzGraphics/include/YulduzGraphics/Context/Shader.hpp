#pragma once

#include <Yulduz/Core.hpp>
#include <Yulduz/Assets.hpp>

#include <YulduzGraphics/Enums.hpp>

namespace Yulduz {
    class FileAsset;
    class GraphicsContext;

    class Shader {
       public:
        Shader(const std::string &label, const WGPUShaderModule &handle);

        Shader();
        ~Shader();

        Shader(const Shader &other);
        Shader(Shader &&other);

        Shader &operator=(const Shader &other);
        Shader &operator=(Shader &&other);

        WGPUShaderModule get() const;
        std::string getLabel() const;

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

        Shader buildWGSL(const std::shared_ptr<FileAsset> &asset, const GraphicsContext &context);
        Shader buildGLSL(const std::shared_ptr<FileAsset> &asset, ShaderStage stages, const GraphicsContext &context);
        Shader buildSPIRV(const std::shared_ptr<FileAsset> &asset, const GraphicsContext &context);

       private:
        std::string m_Label;
    };
}  // namespace Yulduz