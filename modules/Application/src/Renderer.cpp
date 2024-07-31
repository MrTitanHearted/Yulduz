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

namespace Utils {
    std::uint32_t ConvertToRGBA(const glm::vec4& color) {
        std::uint8_t r = static_cast<std::uint8_t>(color.r * 255.0f);
        std::uint8_t g = static_cast<std::uint8_t>(color.g * 255.0f);
        std::uint8_t b = static_cast<std::uint8_t>(color.b * 255.0f);
        std::uint8_t a = static_cast<std::uint8_t>(color.a * 255.0f);

        return (a << 24) | (b << 16) | (g << 8) | r;
    }
}  // namespace Utils

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
                    .setFormat(TextureFormat::RGBA8Unorm)
                    .emptyFramebuffer(width, height, m_Context);
        }

        m_ImageData.resize(width * height);
    }

    void Renderer::render(const RayTracedCamera& camera) {
        const glm::vec3& rayOrigin = camera.GetPosition();

        Ray ray{.Origin = camera.GetPosition()};

        auto [width, height] = m_FinalImage->getSize2D();
        for (std::uint32_t y = 0; y < height; y++) {
            for (std::uint32_t x = 0; x < width; x++) {
                std::size_t i = x + y * width;
                ray.Direction = camera.GetRayDirections()[i];
                glm::vec4 color = glm::clamp(traceRay(ray), glm::vec4{0.0f}, glm::vec4{1.0f});
                m_ImageData[i] = Utils::ConvertToRGBA(color);
            }
        }

        ImageCopyTexture::New(m_FinalImage).write(m_ImageData.data(), m_Context);
    }

    glm::vec4 Renderer::traceRay(const Ray& ray) {
        static float radius = 0.5f;
        // (bx^2 + by^2)t^2 + 2(axbx + ayby)t + (ax^2+ay^2-r^2)=0
        // where
        // a = ray origin
        // b = ray direction
        // r = radius
        // t = hit distance

        // float a = rayDirection.x * rayDirection.x + rayDirection.y * rayDirection.y + rayDirection.z * rayDirection.z;
        // float b = 2.0f * (rayOrigin.x * rayDirection.x + rayOrigin.y * rayDirection.y);
        // float c = rayOrigin.x * rayOrigin.x + rayOrigin.y * rayOrigin.y - radius * radius;
        float a = glm::dot(ray.Direction, ray.Direction);
        float b = 2.0f * glm::dot(ray.Origin, ray.Direction);
        float c = glm::dot(ray.Origin, ray.Origin) - radius * radius;

        // Quadratic formula discriminant:
        // b^2 - 4ac

        float discriminant = b * b - 4.0f * a * c;
        if (discriminant < 0.0f) return glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

        // (-b + sqrt(discriminant)) / (2a)
        float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a);
        float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);

        glm::vec3 hitPoint = ray.Origin + ray.Direction * closestT;
        glm::vec3 normal = glm::normalize(hitPoint);

        static glm::vec3 lightDir = glm::normalize(glm::vec3{-1.0f, -1.0f, -1.0f});

        float d = glm::max(glm::dot(normal, -lightDir), 0.0f);  // == cos(angle)

        glm::vec3 sphereColor{1.0f, 0.0f, 1.0f};
        sphereColor *= d;
        return glm::vec4(sphereColor, 1.0f);
    }
}  // namespace Yulduz