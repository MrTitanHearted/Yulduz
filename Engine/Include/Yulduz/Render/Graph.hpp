#pragma once

#include <Yulduz/Common.hpp>

namespace Yulduz {
    struct TextureAlias {
        size_t Hash = 0;

        TextureAlias()  = default;
        ~TextureAlias() = default;

        TextureAlias(const eastl::string_view name)
            : Hash{eastl::hash<eastl::string_view>{}(name)} {}

        bool operator==(const TextureAlias &handle) const = default;
    };

    struct BufferAlias {
        size_t Hash = 0;

        BufferAlias()  = default;
        ~BufferAlias() = default;

        BufferAlias(const eastl::string_view name)
            : Hash{eastl::hash<eastl::string_view>{}(name)} {}

        bool operator==(const BufferAlias &handle) const = default;
    };

    constexpr static auto k_InvalidTextureAlias = TextureAlias{};
    constexpr static auto k_InvalidBufferAlias  = BufferAlias{};
}  // namespace Yulduz

template <>
struct YULDUZ_API eastl::hash<Yulduz::TextureAlias> {
    size_t operator()(const Yulduz::TextureAlias &handle) const {
        return handle.Hash;
    }
};

template <>
struct YULDUZ_API eastl::hash<Yulduz::BufferAlias> {
    size_t operator()(const Yulduz::BufferAlias &handle) const {
        return handle.Hash;
    }
};

namespace Yulduz {
    class RenderContext {
       public:
        RenderContext()  = default;
        ~RenderContext() = default;

        SDL_GPUCommandBuffer *getCommandBuffer() const;

        SDL_GPUTexture *getTexture(const TextureAlias &alias) const;
        SDL_GPUBuffer  *getBuffer(const BufferAlias &alias) const;

       private:
        SDL_GPUCommandBuffer *m_CommandBuffer = nullptr;

        eastl::hash_map<TextureAlias, SDL_GPUTexture *> m_Textures;
        eastl::hash_map<BufferAlias, SDL_GPUBuffer *>   m_Buffers;

       private:
        friend class RenderGraph;
    };

    class RenderPass {
       public:
        RenderPass()  = default;
        ~RenderPass() = default;

        RenderPass &setName(eastl::string_view name);

        RenderPass &readTexture(const TextureAlias &alias);
        RenderPass &writeTexture(const TextureAlias &alias);
        RenderPass &readBuffer(const BufferAlias &alias);
        RenderPass &writeBuffer(const BufferAlias &alias);

        RenderPass &setExecuteFn(eastl::function<void(const RenderContext &)> fn);

       private:
        eastl::string m_Name{};

        eastl::vector<TextureAlias> m_ReadTextures{};
        eastl::vector<TextureAlias> m_WriteTextures{};

        eastl::vector<BufferAlias> m_ReadBuffers{};
        eastl::vector<BufferAlias> m_WriteBuffers{};

        eastl::function<void(const RenderContext &)> m_ExecuteFn = nullptr;

       private:
        friend class RenderGraph;
    };

    class RenderGraph {
       public:
        void setTexture(const TextureAlias &alias, SDL_GPUTexture *texture);
        void setBuffer(const BufferAlias &alias, SDL_GPUBuffer *buffer);

        void addRenderPass(const RenderPass &render_pass);

       private:
        struct ExecutorPass {
            eastl::function<void(const RenderContext &)> ExecuteFn;

            RenderContext Context;
        };

        struct Executor {
            eastl::vector<ExecutorPass> Passes;
        };

       private:
        static void Execute(SDL_GPUCommandBuffer *command_buffer, Executor &&executor);

       private:
        RenderGraph()  = default;
        ~RenderGraph() = default;

        void setSwapchainTextureAlias(const TextureAlias &alias);

        Executor buildExecutor(SDL_GPUTexture *swapchain_texture);

       private:
        TextureAlias m_SwapchainTextureAlias = k_InvalidTextureAlias;

        eastl::vector<RenderPass> m_RenderPasses{};

        eastl::hash_map<TextureAlias, SDL_GPUTexture *> m_Textures{};
        eastl::hash_map<BufferAlias, SDL_GPUBuffer *>   m_Buffers{};

       private:
        friend class Engine;
    };
}  // namespace Yulduz