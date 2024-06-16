#pragma once

#include <Yulduz/Core.hpp>
#include <YulduzAssets/IAsset.hpp>

namespace Yulduz {
    class TextureAsset : public IAsset {
       public:
        static std::optional<std::shared_ptr<TextureAsset>> FromData(const std::string &path, const void *data, size_t size, bool flipVertically = true);
        static std::optional<std::shared_ptr<TextureAsset>> FromPath(const std::string &path, bool flipVertically = true);

       public:
        TextureAsset(const std::string &path, void *data, size_t size, uint32_t width, uint32_t height, uint32_t channelCount, uint32_t stride);
        ~TextureAsset();

        std::string getName() const;
        std::string getPath() const;
        std::string getDirectory() const;
        std::string getExtension() const;
        const void *getData() const;
        size_t getSize() const;
        uint32_t getWidth() const;
        uint32_t getHeight() const;
        uint32_t getChannelCount() const;
        uint32_t getStride() const;

       private:
        std::string m_Name;
        std::string m_Path;
        std::string m_Directory;
        std::string m_Extension;
        void *m_Data;
        size_t m_Size;
        uint32_t m_Width;
        uint32_t m_Height;
        uint32_t m_ChannelCount;
        uint32_t m_Stride;
    };
}  // namespace Yulduz
