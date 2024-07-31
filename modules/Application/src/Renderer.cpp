#include <Yulduz/Renderer.hpp>
#include <random>

namespace Random {
    std::uint32_t Uint32() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
#undef max
        static std::uniform_int_distribution<std::uint32_t> distr(0, std::numeric_limits<std::uint32_t>::max());
        return distr(gen);
    }
}  // namespace Random

namespace Yulduz {
    void Renderer::resize(std::uint32_t width, std::uint32_t height) {
        if (m_FinalImage) {
            if (m_FinalImage->getWidth() == width && m_FinalImage->getHeight() == height)
                return;

            m_FinalImage->resize2D(width, height, m_Context);
        } else if (width != 0 && height != 0) {
            m_FinalImage =
                TextureBuilder::New()
                    .setLabel("Renderer Image")
                    .addTextureUsage(TextureUsage::CopySrc)
                    .setFormat(TextureFormat::RGBA8UnormSrgb)
                    .emptyFramebuffer(width, height, m_Context);
        }

        m_ImageData.resize(width * height);
    }

    void Renderer::render() {
        auto [width, height] = m_FinalImage->getSize2D();
        for (std::uint32_t y = 0; y < height; y++) {
            for (std::uint32_t x = 0; x < width; x++) {
                glm::vec2 coord{static_cast<float>(x) / static_cast<float>(width), static_cast<float>(y) / static_cast<float>(height)};
                coord = coord * 2.0f - 1.0f;  // -1 -> 1
                std::size_t i = x + y * width;

                m_ImageData[i] = perPixel(coord);
            }
        }

        ImageCopyTexture::New(m_FinalImage).write(m_ImageData.data(), m_Context);
    }

    std::uint32_t Renderer::perPixel(glm::vec2 coord) {
        float radius = 0.5f;
        glm::vec3 rayOrigin{0.0f, 0.0f, 2.0f};
        glm::vec3 rayDirection{coord.x, coord.y, -1.0f};
        // rayDirection = glm::normalize(rayDirection);

        // (bx^2 + by^2)t^2 + 2(axbx + ayby)t + (ax^2+ay^2-r^2)=0
        // where
        // a = ray origin
        // b = ray direction
        // r = radius
        // t = hit distance

        // float a = rayDirection.x * rayDirection.x + rayDirection.y * rayDirection.y + rayDirection.z * rayDirection.z;
        // float b = 2.0f * (rayOrigin.x * rayDirection.x + rayOrigin.y * rayDirection.y);
        // float c = rayOrigin.x * rayOrigin.x + rayOrigin.y * rayOrigin.y - radius * radius;
        float a = glm::dot(rayDirection, rayDirection);
        float b = 2.0f * glm::dot(rayOrigin, rayDirection);
        float c = glm::dot(rayOrigin, rayOrigin) - radius * radius;

        // Quadratic formula discriminant:
        // b^2 - 4ac

        float discriminant = b * b - 4.0f * a * c;

        if (discriminant < 0) return 0xFF000000;

        return 0xFFFF00FF;
    }
}  // namespace Yulduz