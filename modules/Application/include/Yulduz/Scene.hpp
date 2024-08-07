#pragma once

#include <Yulduz/Engine.hpp>

namespace Yulduz {
    struct Material {
        glm::vec3 Albedo{1.0f};
        float Metallic{0.0f};
        glm::vec3 EmissionColor{0.0f};
        float EmissionPower{0.0f};
        float Roughness{1.0f};
        glm::vec3 _Padding{0.0f};
    };

    struct Sphere {
        glm::vec3 Position{0.0f};
        float Radius{0.5f};
        std::int32_t MaterialIndex{0};
        glm::vec3 _Padding{0.0f};
    };

    struct Scene {
        std::vector<Sphere> Spheres;
        std::vector<Material> Materials;
    };
}  // namespace Yulduz
