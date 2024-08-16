#pragma once

#include <Yulduz/Core.hpp>
#include <YulduzGraphics/Enums.hpp>
#include <YulduzGraphics/Context/Pipeline.hpp>

namespace Yulduz {
    class GraphicsContext;

    class RenderBundle {
       public:
        RenderBundle(const std::string &label,
                     const WGPURenderBundle &renderBundle,
                     const std::vector<RenderPipeline> &renderPipelines,
                     const std::vector<Buffer> &buffers,
                     const std::vector<BindGroup> &bindGroups);

        RenderBundle();
        ~RenderBundle();

        RenderBundle(const RenderBundle &other);
        RenderBundle(RenderBundle &&other);

        RenderBundle &operator=(const RenderBundle &other);
        RenderBundle &operator=(RenderBundle &&other);

        std::string getLabel() const;
        WGPURenderBundle get() const;

       private:
        std::string m_Label;
        WGPURenderBundle m_RenderBundle;
        std::vector<RenderPipeline> m_RenderPipelines;
        std::vector<Buffer> m_Buffers;
        std::vector<BindGroup> m_BindGroups;
    };

    class RenderBundleEncoder {
       public:
        RenderBundleEncoder(const std::string &label, const WGPURenderBundleEncoder &handle);

        RenderBundleEncoder();
        ~RenderBundleEncoder();

        RenderBundleEncoder(const RenderBundleEncoder &other);
        RenderBundleEncoder(RenderBundleEncoder &&other);

        RenderBundleEncoder &operator=(const RenderBundleEncoder &other);
        RenderBundleEncoder &operator=(RenderBundleEncoder &&other);

        void setPipeline(const RenderPipeline &pipeline);
        void setBindGroups(const std::vector<BindGroup> &bindGroups);
        void setBindGroup(std::uint32_t groupIndex, const BindGroup &bindGroup);
        void setVertexBuffers(const std::vector<VertexBuffer> &vertexBuffers);
        void setVertexBuffer(std::uint32_t slot, const VertexBuffer &vertexBuffer);
        
        void draw(std::uint32_t slot, const VertexBuffer &vertexBuffer);
        void drawIndexed(const IndexBuffer &indexBuffer);

        RenderBundle finish(const std::string &label = "Yulduz RenderBundle");

       private:
        std::string m_Label;
        WGPURenderBundleEncoder m_RenderBundleEncoder;
        std::vector<RenderPipeline> m_RenderPipelines;
        std::vector<Buffer> m_Buffers;
        std::vector<BindGroup> m_BindGroups;
    };

    class RenderBundleEncoderBuilder {
       public:
        RenderBundleEncoderBuilder();
        ~RenderBundleEncoderBuilder() = default;

        inline static RenderBundleEncoderBuilder New() { return RenderBundleEncoderBuilder(); }

        RenderBundleEncoderBuilder &setLabel(const std::string &label);
        RenderBundleEncoderBuilder &setColorFormats(const std::vector<TextureFormat> &colorFormats);
        RenderBundleEncoderBuilder &addColorFormat(TextureFormat colorFormat);
        RenderBundleEncoderBuilder &setDepthStencilFormat(TextureFormat depthStencilFormat);
        RenderBundleEncoderBuilder &setSampleCount(std::uint32_t sampleCount);
        RenderBundleEncoderBuilder &setDepthReadOnly(bool depthReadOnly);
        RenderBundleEncoderBuilder &setStencilReadOnly(bool stencilReadOnly);

        RenderBundleEncoder build(const GraphicsContext &context);

       private:
        std::string m_Label;
        std::vector<TextureFormat> m_ColorFormats;
        TextureFormat m_DepthStencilFormat;
        std::uint32_t m_SampleCount;
        bool m_DepthReadOnly;
        bool m_StencilReadOnly;
    };
}  // namespace Yulduz
