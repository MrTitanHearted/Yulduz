#pragma once

#include <Yulduz/Common.hpp>

namespace Yulduz {
    class YULDUZ_API TextureAsset {
       public:
        enum class Type {
            eRGBA32f,
            eRGB32f,
            eRGBA16u,
            eRGB16u,
            eRGBA8u,
            eRGB8u,
        };

       public:
        static eastl::optional<TextureAsset> LoadFromPath(eastl::string_view path, Type type = Type::eRGBA8u);
        static eastl::optional<TextureAsset> LoadFromMemory(const void *data, size_t size, Type type = Type::eRGBA8u);


        static glm::u32 GetChannelCount(Type type);
        static glm::u32 GetTexelSize(Type type);

        static void SetFlipVertically(bool flip);

       public:
        TextureAsset()  = default;
        ~TextureAsset() = default;

        glm::u32 getWidth() const;
        glm::u32 getHeight() const;

        Type getType() const;

        eastl::span<const glm::u8> getDataConst() const;

        eastl::span<glm::u8> getData();

       private:
        glm::u32 m_Width;
        glm::u32 m_Height;

        Type m_Type;

        eastl::vector<glm::u8> m_Data;
    };
}  // namespace Yulduz