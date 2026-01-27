#include <Yulduz/Assets/TextureAsset.hpp>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_SIMD
#include <stb_image.h>

namespace Yulduz {
    eastl::optional<TextureAsset> TextureAsset::LoadFromPath(const eastl::string_view path, const Type type) {
        if (!SDL_GetPathInfo(path.data(), nullptr)) {
            DYULDUZ_LOG_ENGINE_WARN("Failed to find a texture from path: '{}'.", path);
            return eastl::nullopt;
        }

        const char *texture_path = path.data();

        glm::i32 width  = 0;
        glm::i32 height = 0;

        const glm::i32 desired_channels = GetChannelCount(type);
        const glm::i32 texel_size       = GetTexelSize(type);

        uint8_t *texture_data = nullptr;

        switch (type) {
            case Type::eRGBA32f:
            case Type::eRGB32f: {
                texture_data =
                    reinterpret_cast<uint8_t *>(stbi_loadf(texture_path, &width, &height, nullptr, desired_channels));
            } break;
            case Type::eRGBA16u:
            case Type::eRGB16u: {
                texture_data =
                    reinterpret_cast<uint8_t *>(stbi_load_16(texture_path, &width, &height, nullptr, desired_channels));
            } break;
            case Type::eRGBA8u:
            case Type::eRGB8u: {
                texture_data = stbi_load(texture_path, &width, &height, nullptr, desired_channels);

            } break;
        }

        const size_t texture_size = width * height * texel_size;

        TextureAsset texture_asset{};
        texture_asset.m_Width  = width;
        texture_asset.m_Height = height;
        texture_asset.m_Type   = type;
        texture_asset.m_Data.resize(texture_size);
        std::memcpy(texture_asset.m_Data.data(), texture_data, texture_size);

        stbi_image_free(texture_data);

        DYULDUZ_LOG_ENGINE_INFO("Loaded a Yulduz::TextureAsset from path: '{}'", texture_path);

        return texture_asset;
    }

    eastl::optional<TextureAsset> TextureAsset::LoadFromMemory(const void *data, const size_t size, const Type type) {
        glm::i32 width  = 0;
        glm::i32 height = 0;

        const glm::i32 desired_channels = GetChannelCount(type);
        const glm::i32 texel_size       = GetTexelSize(type);

        uint8_t *texture_data = nullptr;

        switch (type) {
            case Type::eRGBA32f:
            case Type::eRGB32f: {
                texture_data =
                    reinterpret_cast<uint8_t *>(stbi_loadf_from_memory(
                        static_cast<stbi_uc const *>(data), size, &width, &height, nullptr, desired_channels));
            } break;
            case Type::eRGBA16u:
            case Type::eRGB16u: {
                texture_data =
                    reinterpret_cast<uint8_t *>(stbi_load_16_from_memory(
                        static_cast<stbi_uc const *>(data), size, &width, &height, nullptr, desired_channels));
            } break;
            case Type::eRGBA8u:
            case Type::eRGB8u: {
                texture_data = stbi_load_from_memory(
                    static_cast<stbi_uc const *>(data), size, &width, &height, nullptr, desired_channels);

            } break;
        }

        const size_t texture_size = width * height * texel_size;

        TextureAsset texture_asset{};
        texture_asset.m_Width  = width;
        texture_asset.m_Height = height;
        texture_asset.m_Type   = type;
        texture_asset.m_Data.resize(texture_size);
        std::memcpy(texture_asset.m_Data.data(), texture_data, texture_size);

        stbi_image_free(texture_data);

        DYULDUZ_LOG_ENGINE_INFO("Loaded a Yulduz::TextureAsset from memory: '{:p}'", data);

        return texture_asset;
    }

    glm::u32 TextureAsset::GetChannelCount(const Type type) {
        switch (type) {
            case Type::eRGBA32f:
            case Type::eRGBA16u:
            case Type::eRGBA8u:
                return 4;
            case Type::eRGB32f:
            case Type::eRGB16u:
            case Type::eRGB8u:
                return 3;
        }
        return 0;
    }

    glm::u32 TextureAsset::GetTexelSize(const Type type) {
        switch (type) {
            case Type::eRGBA32f:
                return 4 * sizeof(float);
            case Type::eRGB32f:
                return 3 * sizeof(float);
            case Type::eRGBA16u:
                return 4 * sizeof(uint16_t);
            case Type::eRGB16u:
                return 3 * sizeof(uint16_t);
            case Type::eRGBA8u:
                return 4 * sizeof(uint8_t);
            case Type::eRGB8u:
                return 3 * sizeof(uint8_t);
        }
        return 0;
    }

    void TextureAsset::SetFlipVertically(const bool flip) {
        stbi_set_flip_vertically_on_load(flip);
    }

    glm::u32 TextureAsset::getWidth() const {
        return m_Width;
    }

    glm::u32 TextureAsset::getHeight() const {
        return m_Height;
    }

    TextureAsset::Type TextureAsset::getType() const {
        return m_Type;
    }

    eastl::span<const glm::u8> TextureAsset::getDataConst() const {
        return m_Data;
    }

    eastl::span<glm::u8> TextureAsset::getData() {
        return m_Data;
    }
}  // namespace Yulduz