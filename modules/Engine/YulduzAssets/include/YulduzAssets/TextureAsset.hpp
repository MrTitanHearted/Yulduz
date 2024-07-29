#pragma once

#include <Yulduz/Core.hpp>
#include <YulduzAssets/IAsset.hpp>

namespace Yulduz {
    class TextureAsset : public IAsset {
       public:
        static std::optional<std::shared_ptr<TextureAsset>> FromData(const fs::path &path, void *data, std::size_t size, bool flipVertically = true);
        static std::optional<std::shared_ptr<TextureAsset>> FromPath(const fs::path &path, bool flipVertically = true);

       public:
        TextureAsset(const fs::path &path, void *data, std::size_t size, std::uint32_t width, std::uint32_t height, std::uint32_t channelCount, std::uint32_t stride);
        ~TextureAsset();

        fs::path getPath() const;
        fs::path getDirectory() const;
        std::string getName() const;
        std::string getExtension() const;
        void *getData() const;
        std::size_t getSize() const;
        std::uint32_t getWidth() const;
        std::uint32_t getHeight() const;
        std::uint32_t getChannelCount() const;
        std::uint32_t getStride() const;

       private:
        fs::path m_Path;
        fs::path m_Directory;
        std::string m_Name;
        std::string m_Extension;
        void *m_Data;
        std::size_t m_Size;
        std::uint32_t m_Width;
        std::uint32_t m_Height;
        std::uint32_t m_ChannelCount;
        std::uint32_t m_Stride;
    };
}  // namespace Yulduz