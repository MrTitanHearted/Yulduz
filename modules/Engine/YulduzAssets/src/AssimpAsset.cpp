#include <YulduzAssets/AssimpAsset.hpp>

namespace Yulduz {
    AssimpAsset::AssimpAsset(const fs::path &path, aiScene *scene)
        : m_Path(path), m_Scene(scene) {
        YZDEBUG("Initializing Assimp Asset: '{}'", m_Path.string());

        m_Directory = m_Path.parent_path();
        m_Name = m_Path.filename().string();
        m_Extension = m_Path.extension().string();
    };

    AssimpAsset::~AssimpAsset() {
        YZDEBUG("Releasing Assimp Asset: '{}'", m_Path.string());

        aiReleaseImport(m_Scene);
    }

    fs::path AssimpAsset::getPath() { return m_Path; }

    fs::path AssimpAsset::getDirectory() { return m_Directory; }

    std::string AssimpAsset::getName() { return m_Name; }

    std::string AssimpAsset::getExtension() { return m_Extension; }

    aiScene *AssimpAsset::getScene() { return m_Scene; }

    std::optional<std::shared_ptr<AssimpAsset>> AssimpAsset::FromPath(const fs::path &path, AssimpPostProcess postProcess) {
        if (!fs::exists(path)) {
            YZERROR("Failed to find an Assimp Asset from path: '{}'", path.string());
            return std::nullopt;
        }

        aiScene *scene = (aiScene *)aiImportFile(path.string().c_str(), aiProcess_Triangulate | aiProcess_MakeLeftHanded | aiProcess_JoinIdenticalVertices | static_cast<std::uint32_t>(postProcess));
        if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr) {
            YZERROR("Failed to load an Assimp Asset from path: '{}' because of '{}'", path.string(), aiGetErrorString());
            return std::nullopt;
        }

        return std::make_shared<AssimpAsset>(path, scene);
    }

}  // namespace Yulduz