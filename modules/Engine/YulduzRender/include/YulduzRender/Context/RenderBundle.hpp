#pragma once

#include <Yulduz/Core.hpp>
#include <YulduzRender/Enums.hpp>

namespace Yulduz {
    class RenderContext;
    class RenderPipeline;
    class VertexBuffer;
    class IndexBuffer;
    class Buffer;
    class BindGroup;

    class RenderBundle {
       public:
        RenderBundle(const std::string &label,
                     const WGPURenderBundle &renderBundle,
                     const std::vector<std::shared_ptr<RenderPipeline>> &pipelines,
                     const std::vector<std::shared_ptr<Buffer>> &buffers,
                     const std::vector<std::shared_ptr<BindGroup>> &bindGroups);
        ~RenderBundle();

        std::string getLabel() const;
        WGPURenderBundle get() const;

       private:
        std::string m_Label;
        WGPURenderBundle m_RenderBundle;
        std::vector<std::shared_ptr<RenderPipeline>> m_RenderPipelines;
        std::vector<std::shared_ptr<Buffer>> m_Buffers;
        std::vector<std::shared_ptr<BindGroup>> m_BindGroups;
    };

    class RenderBundleEncoder {
       public:
        RenderBundleEncoder(const std::string &label, const WGPURenderBundleEncoder &encoder);
        ~RenderBundleEncoder();

        void setRenderPipeline(const std::shared_ptr<RenderPipeline> &pipeline);
        void setVertexBuffers(const std::vector<std::shared_ptr<VertexBuffer>> &buffers);
        void setVertexBuffer(uint32_t slot, const std::shared_ptr<VertexBuffer> &buffer);
        void setBindGroups(const std::vector<std::shared_ptr<BindGroup>> &bindGroups);
        void setBindGroup(uint32_t groupIndex, const std::shared_ptr<BindGroup> &bindGroup);

        void draw(uint32_t slot, const std::shared_ptr<VertexBuffer> &buffer);
        void drawIndexed(const std::shared_ptr<IndexBuffer> &buffer);

        std::string getLabel() const;
        WGPURenderBundleEncoder get() const;

        std::shared_ptr<RenderBundle> finish(const std::string &label = "Yulduz Render Bundle");

       private:
        std::string m_Label;
        WGPURenderBundleEncoder m_RenderBundleEncoder;
        std::vector<std::shared_ptr<RenderPipeline>> m_RenderPipelines;
        std::vector<std::shared_ptr<Buffer>> m_Buffers;
        std::vector<std::shared_ptr<BindGroup>> m_BindGroups;
    };

    class RenderBundleEncoderBuilder {
       public:
        RenderBundleEncoderBuilder();
        ~RenderBundleEncoderBuilder() = default;

        inline static RenderBundleEncoderBuilder New() { return RenderBundleEncoderBuilder(); }

        RenderBundleEncoderBuilder &setLabel(const std::string &label);
        RenderBundleEncoderBuilder &setColorFormats(const std::vector<TextureFormat> &colorFormats);
        RenderBundleEncoderBuilder &addColorFormats(const std::vector<TextureFormat> &colorFormats);
        RenderBundleEncoderBuilder &addColorFormat(TextureFormat colorFormat);
        RenderBundleEncoderBuilder &setDepthStencilFormat(TextureFormat depthStencilFormat);
        RenderBundleEncoderBuilder &setSampleCount(uint32_t sampleCount);
        RenderBundleEncoderBuilder &setDepthReadOnly(bool depthReadOnly);
        RenderBundleEncoderBuilder &setStencilReadOnly(bool stencilReadOnly);

        std::shared_ptr<RenderBundleEncoder> build(const std::shared_ptr<RenderContext> &context);

       private:
        std::string m_Label;
        std::vector<WGPUTextureFormat> m_ColorFormats;
        TextureFormat m_DepthStencilFormat;
        uint32_t m_SampleCount;
        bool m_DepthReadOnly;
        bool m_StencilReadOnly;
    };
}  // namespace Yulduz
