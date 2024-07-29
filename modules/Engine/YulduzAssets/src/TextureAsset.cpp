#include <YulduzAssets/TextureAsset.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Yulduz {
    TextureAsset::TextureAsset(const fs::path &path, void *data, std::size_t size, std::uint32_t width, std::uint32_t height, std::uint32_t channelCount, std::uint32_t stride)
        : m_Path(path), m_Data(data), m_Size(size), m_Width(width), m_Height(height), m_ChannelCount(channelCount), m_Stride(stride) {
        YZDEBUG("Initializing Texture Asset: '{}'", m_Path.string());

        m_Directory = m_Path.parent_path();
        m_Name = m_Path.filename().string();
        m_Extension = m_Path.extension().string();
    }

    TextureAsset::~TextureAsset() {
        YZDEBUG("Releasing Texture Asset: '{}'", m_Path.string());

        stbi_image_free(m_Data);
    }

    fs::path TextureAsset::getPath() const { return m_Path; }

    fs::path TextureAsset::getDirectory() const { return m_Directory; }

    std::string TextureAsset::getName() const { return m_Name; }

    std::string TextureAsset::getExtension() const { return m_Extension; }

    void *TextureAsset::getData() const { return m_Data; }

    std::size_t TextureAsset::getSize() const { return m_Size; }

    std::uint32_t TextureAsset::getWidth() const { return m_Width; }

    std::uint32_t TextureAsset::getHeight() const { return m_Height; }

    std::uint32_t TextureAsset::getChannelCount() const { return m_ChannelCount; }

    std::uint32_t TextureAsset::getStride() const { return m_Stride; }

    std::optional<std::shared_ptr<TextureAsset>> TextureAsset::FromData(const fs::path &path, void *data, std::size_t size, bool flipVertically) {
        if (flipVertically) stbi_set_flip_vertically_on_load(1);

        std::int32_t width, height;
        void *textureData = stbi_load_from_memory((const stbi_uc *)data, size, &width, &height, nullptr, STBI_rgb_alpha);
        if (textureData == nullptr) {
            YZERROR("Failed to load a Texture Asset of path: '{}' from memory: '{:#X}' because of '{}'", path.string(), data, stbi_failure_reason());
            return std::nullopt;
        }

        if (flipVertically) stbi_set_flip_vertically_on_load(0);

        return std::make_shared<TextureAsset>(path, textureData, width * height * 4, width, height, 4, width * 4);
    }

    std::optional<std::shared_ptr<TextureAsset>> TextureAsset::FromPath(const fs::path &path, bool flipVertically) {
        if (!fs::exists(path)) {
            YZERROR("Failed to find a Texture Asset from path: '{}'", path.string());
            return std::nullopt;
        }

        if (flipVertically) stbi_set_flip_vertically_on_load(1);

        std::int32_t width, height;
        void *data = stbi_load(path.string().c_str(), &width, &height, nullptr, STBI_rgb_alpha);
        if (data == nullptr) {
            YZERROR("Failed to load a Texture Asset from path: '{}' because of '{}'", path.string(), stbi_failure_reason());
            return std::nullopt;
        }

        if (flipVertically) stbi_set_flip_vertically_on_load(0);

        return std::make_shared<TextureAsset>(path, data, width * height * 4, width, height, 4, width * 4);
    }
}  // namespace Yulduz
