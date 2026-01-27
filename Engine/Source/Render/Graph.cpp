#include <Yulduz/Render/Graph.hpp>

namespace Yulduz {
    SDL_GPUCommandBuffer *RenderContext::getCommandBuffer() const {
        return m_CommandBuffer;
    }

    SDL_GPUTexture *RenderContext::getTexture(const TextureAlias &alias) const {
        if (const auto it = m_Textures.find(alias);
            it != eastl::end(m_Textures)) {
            return it->second;
        }
        DYULDUZ_LOG_ENGINE_WARN("Texture with alias '{}' does not exist in this render context", alias.Hash);
        return nullptr;
    }

    SDL_GPUBuffer *RenderContext::getBuffer(const BufferAlias &alias) const {
        if (const auto it = m_Buffers.find(alias);
            it != eastl::end(m_Buffers)) {
            return it->second;
        }
        DYULDUZ_LOG_ENGINE_WARN("Buffer with alias '{}' does not exist in this render context", alias.Hash);
        return nullptr;
    }

    RenderPass &RenderPass::setName(const eastl::string_view name) {
        m_Name = name;
        return *this;
    }

    RenderPass &RenderPass::readTexture(const TextureAlias &alias) {
        m_ReadTextures.emplace_back(alias);
        return *this;
    }

    RenderPass &RenderPass::writeTexture(const TextureAlias &alias) {
        m_WriteTextures.emplace_back(alias);
        return *this;
    }

    RenderPass &RenderPass::readBuffer(const BufferAlias &alias) {
        m_ReadBuffers.emplace_back(alias);
        return *this;
    }

    RenderPass &RenderPass::writeBuffer(const BufferAlias &alias) {
        m_WriteBuffers.emplace_back(alias);
        return *this;
    }

    RenderPass &RenderPass::setExecuteFn(eastl::function<void(const RenderContext &)> fn) {
        m_ExecuteFn = fn;
        return *this;
    }

    void RenderGraph::setTexture(const TextureAlias &alias, SDL_GPUTexture *texture) {
        if (k_InvalidTextureAlias == alias) {
            DYULDUZ_LOG_ENGINE_WARN("Invalid Texture Alias");
            return;
        }
        if (nullptr == texture) {
            m_Textures.erase(alias);
            return;
        }
        m_Textures[alias] = texture;
    }

    void RenderGraph::setBuffer(const BufferAlias &alias, SDL_GPUBuffer *buffer) {
        if (k_InvalidBufferAlias == alias) {
            DYULDUZ_LOG_ENGINE_WARN("Invalid Buffer Alias");
            return;
        }
        if (nullptr == buffer) {
            m_Buffers.erase(alias);
            return;
        }
        m_Buffers[alias] = buffer;
    }

    void RenderGraph::addRenderPass(const RenderPass &render_pass) {
        m_RenderPasses.emplace_back(render_pass);
    }

    void RenderGraph::Execute(SDL_GPUCommandBuffer *command_buffer, Executor &&executor) {
        for (auto &[execute, context] : executor.Passes) {
            context.m_CommandBuffer = command_buffer;
            execute(context);
        }
    }

    void RenderGraph::setSwapchainTextureAlias(const TextureAlias &alias) {
        m_SwapchainTextureAlias = alias;
    }

    RenderGraph::Executor RenderGraph::buildExecutor(SDL_GPUTexture *swapchain_texture) {
        setTexture(m_SwapchainTextureAlias, swapchain_texture);

        eastl::vector<ExecutorPass> passes{};

        const size_t pass_count = m_RenderPasses.size();

        eastl::vector<size_t> order{};
        order.reserve(pass_count);

        // Build dependency graph using adjacency list
        eastl::vector<eastl::vector<size_t>> adjacency_list(pass_count);
        eastl::vector<size_t>                in_degree(pass_count, 0);

        // Track which passes write to each resource
        eastl::hash_map<TextureAlias, eastl::vector<size_t>> texture_writers;
        eastl::hash_map<BufferAlias, eastl::vector<size_t>>  buffer_writers;

        // First pass: identify all writers
        for (size_t i = 0; i < pass_count; ++i) {
            const auto &pass = m_RenderPasses[i];

            for (const auto &write : pass.m_WriteTextures) {
                texture_writers[write].push_back(i);
            }
            for (const auto &write : pass.m_WriteBuffers) {
                buffer_writers[write].push_back(i);
            }
        }

        // Second pass: build edges based on read-after-write and write-after-write dependencies
        for (size_t i = 0; i < pass_count; ++i) {
            const auto &pass = m_RenderPasses[i];

            eastl::hash_set<size_t> dependencies;

            // Read-after-write dependencies
            for (const auto &read : pass.m_ReadTextures) {
                if (const auto it = texture_writers.find(read); it != texture_writers.end()) {
                    for (const auto writer : it->second) {
                        if (writer < i) {
                            dependencies.insert(writer);
                        }
                    }
                }
            }
            for (const auto &read : pass.m_ReadBuffers) {
                if (const auto it = buffer_writers.find(read); it != buffer_writers.end()) {
                    for (const auto writer : it->second) {
                        if (writer < i) {
                            dependencies.insert(writer);
                        }
                    }
                }
            }

            // Write-after-write dependencies
            for (const auto &write : pass.m_WriteTextures) {
                if (const auto it = texture_writers.find(write); it != texture_writers.end()) {
                    for (const auto writer : it->second) {
                        if (writer < i) {
                            dependencies.insert(writer);
                        }
                    }
                }
            }
            for (const auto &write : pass.m_WriteBuffers) {
                if (const auto it = buffer_writers.find(write); it != buffer_writers.end()) {
                    for (const auto writer : it->second) {
                        if (writer < i) {
                            dependencies.insert(writer);
                        }
                    }
                }
            }

            // Add edges from dependencies to current pass
            for (const auto dependency : dependencies) {
                adjacency_list[dependency].push_back(i);
                in_degree[i]++;
            }
        }

        // Kahn's algorithm for topological sort
        eastl::vector<size_t> queue;
        for (size_t i = 0; i < pass_count; ++i) {
            if (in_degree[i] == 0) {
                queue.push_back(i);
            }
        }

        while (!queue.empty()) {
            const size_t current = queue.back();
            queue.pop_back();
            order.push_back(current);

            for (const auto neighbor : adjacency_list[current]) {
                in_degree[neighbor]--;
                if (in_degree[neighbor] == 0) {
                    queue.push_back(neighbor);
                }
            }
        }

        DYULDUZ_ASSERT(
            order.size() == pass_count,
            "Yulduz::RenderGraph contains cycles! Cannot build executor.");

        for (const auto &i : order) {
            const auto &render_pass = m_RenderPasses[i];

            RenderContext context{};
            for (const auto &read : render_pass.m_ReadTextures) {
                context.m_Textures.emplace(read, m_Textures[read]);
            }
            for (const auto &write : render_pass.m_WriteTextures) {
                context.m_Textures.emplace(write, m_Textures[write]);
            }
            for (const auto &read : render_pass.m_ReadBuffers) {
                context.m_Buffers.emplace(read, m_Buffers[read]);
            }
            for (const auto &write : render_pass.m_WriteBuffers) {
                context.m_Buffers.emplace(write, m_Buffers[write]);
            }

            ExecutorPass pass{
                render_pass.m_ExecuteFn,
                context,
            };

            passes.emplace_back(pass);
        }

        m_RenderPasses.clear();

        return {passes};
    }
}  // namespace Yulduz