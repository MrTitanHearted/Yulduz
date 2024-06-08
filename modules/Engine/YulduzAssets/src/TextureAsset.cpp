#include <YulduzAssets/TextureAsset.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Yulduz {
    TextureAsset::TextureAsset(const std::string &path, void *data, size_t size, uint32_t width, uint32_t height, uint32_t channelCount, uint32_t stride) {
        YZDEBUG("Initializing Texture Asset: '{}'", path);

        m_Path = path;
        m_Data = std::unique_ptr<uint8_t>((uint8_t *)data);
        m_Size = size;
        m_Width = width;
        m_Height = height;
        m_ChannelCount = channelCount;
        m_Stride = stride;

        fs::path fpath(path);
        m_Name = fpath.filename().string();
        m_Directory = fpath.parent_path().string();
        m_Extension = fpath.extension().string();
    }

    TextureAsset::~TextureAsset() {
        YZDEBUG("Releasing Texture Asset: '{}'", m_Path);
    }

    std::string TextureAsset::getName() const {
        return m_Name;
    }

    std::string TextureAsset::getPath() const {
        return m_Path;
    }

    std::string TextureAsset::getDirectory() const {
        return m_Directory;
    }

    std::string TextureAsset::getExtension() const {
        return m_Extension;
    }

    const void *TextureAsset::getData() const {
        return m_Data.get();
    }

    size_t TextureAsset::getSize() const {
        return m_Size;
    }

    uint32_t TextureAsset::getWidth() const {
        return m_Width;
    }

    uint32_t TextureAsset::getHeight() const {
        return m_Height;
    }

    uint32_t TextureAsset::getChannelCount() const {
        return m_ChannelCount;
    }

    uint32_t TextureAsset::getStride() const {
        return m_Stride;
    }

    void releaseTexture(TextureAsset *asset) {
        if (asset == nullptr) return;
        stbi_image_free((void *)asset->getData());
        delete asset;
    }
    
    std::optional<std::shared_ptr<TextureAsset>> TextureAsset::FromData(const std::string &path, const void *data, size_t size, bool flipVertically) {
        if (flipVertically) {
            stbi_set_flip_vertically_on_load(1);
        }

        int width, height;
        void *textureData = stbi_load_from_memory((const stbi_uc *)data, size, &width, &height, nullptr, STBI_rgb_alpha);
        if (textureData == nullptr) {
            YZERROR("Failed to load a Texture Asset of path: '{}' from memory: '{}' because of '{}'", path, data, stbi_failure_reason());
            return std::nullopt;
        }

        if (flipVertically) {
            stbi_set_flip_vertically_on_load(0);
        }

        return std::shared_ptr<TextureAsset>(new TextureAsset(path, textureData, width * height * 4, width, height, 4, width * 4), releaseTexture);
    }

    std::optional<std::shared_ptr<TextureAsset>> TextureAsset::FromPath(const std::string &path, bool flipVertically) {
        if (!fs::exists(path)) {
            YZERROR("Failed to find a Texture Asset from path: '{}'", path);
            return std::nullopt;
        }

        if (flipVertically) {
            stbi_set_flip_vertically_on_load(1);
        }

        int width, height;
        void *data = stbi_load(path.c_str(), &width, &height, nullptr, STBI_rgb_alpha);
        if (data == nullptr) {
            YZERROR("Failed to load a Texture Asset from path: '{}' because of '{}'", path, stbi_failure_reason());
            return std::nullopt;
        }

        if (flipVertically) {
            stbi_set_flip_vertically_on_load(0);
        }

        return std::shared_ptr<TextureAsset>(new TextureAsset(path, data, width * height * 4, width, height, 4, width * 4), releaseTexture);
    }
}  // namespace Yulduz
