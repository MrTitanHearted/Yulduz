#include <Yulduz/Renderer.hpp>
#include <random>

namespace Random {
#undef max
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uint32_t Uint32() {
        static std::uniform_int_distribution<std::uint32_t> distr(0, std::numeric_limits<std::uint32_t>::max());
        return distr(gen);
    }

    double Float64() {
        static std::uniform_real_distribution<double> distr(0.0, 1.0);
        return distr(gen);
    }

    float Float32() {
        static std::uniform_real_distribution<float> distr(0.0f, 1.0f);
        return distr(gen);
    }

    glm::vec3 Vec3(float min, float max) {
        return glm::vec3{
            Float32() * (max - min) + min,
            Float32() * (max - min) + min,
            Float32() * (max - min) + min,
        };
    }
}  // namespace Random

namespace Utils {
    std::uint32_t ConvertToRGBA(const glm::vec4 &color) {
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

    void Renderer::render(const Scene &scene, const RayTracedCamera &camera) {
        m_ActiveScene = &scene;
        m_ActiveCamera = &camera;

        auto [width, height] = m_FinalImage->getSize2D();
        for (std::uint32_t y = 0; y < height; y++) {
            for (std::uint32_t x = 0; x < width; x++) {
                glm::vec4 color = glm::clamp(perPixel(x, y), glm::vec4{0.0f}, glm::vec4{1.0f});
                m_ImageData[x + y * width] = Utils::ConvertToRGBA(color);
            }
        }

        ImageCopyTexture::New(m_FinalImage).write(m_ImageData.data(), m_Context);
    }

    glm::vec4 Renderer::perPixel(std::uint32_t x, std::uint32_t y) {
        const RayTracedCamera &camera = *m_ActiveCamera;
        Ray ray{
            .Origin = camera.GetPosition(),
            .Direction = camera.GetRayDirections()[x + y * m_FinalImage->getWidth()],
        };

        glm::vec3 color{0.0f};
        float multiplier = 1.0f;

        std::size_t bounces = 5;
        for (std::size_t i = 0; i < bounces; i++) {
            HitPayload payload = traceRay(ray);

            if (payload.HitDistance < 0) {
                glm::vec3 skyColor{0.6f, 0.7f, 0.9f};
                color += skyColor * multiplier;
                break;
            }

            glm::vec3 lightDir = glm::normalize(glm::vec3(-1.0f));
            float lightIntensity = glm::max(glm::dot(payload.WorldNormal, -lightDir), 0.0f);

            const Sphere &sphere = m_ActiveScene->Spheres[payload.ObjectIndex];
            const Material &material = m_ActiveScene->Materials[sphere.MaterialIndex];
            glm::vec3 sphereColor = material.Albedo;
            sphereColor *= lightIntensity;
            color += sphereColor * multiplier;

            multiplier *= 0.5f;

            ray.Origin = payload.WorldPosition + payload.WorldNormal * 0.0001f;
            ray.Direction = glm::reflect(ray.Direction, payload.WorldNormal + material.Roughness * Random::Vec3(-0.5f, 0.5f));
        }

        return glm::vec4(color, 1.0f);
    }

    Renderer::HitPayload Renderer::traceRay(const Ray &ray) {
        std::int32_t closestSphere = -1;
#undef max
        float hitDistance = std::numeric_limits<float>::max();

        for (std::size_t i = 0; i < m_ActiveScene->Spheres.size(); i++) {
            const Sphere &sphere = m_ActiveScene->Spheres[i];
            glm::vec3 origin = ray.Origin - sphere.Position;

            float a = glm::dot(ray.Direction, ray.Direction);
            float b = 2.0f * glm::dot(origin, ray.Direction);
            float c = glm::dot(origin, origin) - sphere.Radius * sphere.Radius;

            float discriminant = b * b - 4.0f * a * c;
            if (discriminant < 0.0f) continue;

            float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);
            if (closestT < hitDistance && closestT > 0.0f) {
                hitDistance = closestT;
                closestSphere = i;
            }
        }

        if (closestSphere < 0) return miss(ray);

        return closestHit(ray, hitDistance, closestSphere);
    }

    Renderer::HitPayload Renderer::closestHit(const Ray &ray, float hitDistance, std::int32_t objectIndex) {
        const Sphere &closestSphere = m_ActiveScene->Spheres[objectIndex];

        HitPayload payload{
            .HitDistance = hitDistance,
            .ObjectIndex = objectIndex,
        };

        glm::vec3 origin = ray.Origin - closestSphere.Position;
        payload.WorldPosition = origin + ray.Direction * hitDistance;
        payload.WorldNormal = glm::normalize(payload.WorldPosition);
        payload.WorldPosition += closestSphere.Position;

        return payload;
    }

    Renderer::HitPayload Renderer::miss(const Ray &ray) {
        return HitPayload{.HitDistance = -1.0f};
    }
}  // namespace Yulduz