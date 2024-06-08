#include <YulduzAssets/AssimpAsset.hpp>

namespace Yulduz {
    AssimpAsset::AssimpAsset(const std::string &path, const aiScene *scene) {
        YZDEBUG("Initializing Assimp Asset: '{}'", path);

        m_Path = path;
        m_Scene = std::unique_ptr<aiScene>((aiScene *)scene);

        fs::path fpath(path);
        m_Name = fpath.filename().string();
        m_Directory = fpath.parent_path().string();
        m_Extension = fpath.extension().string();
    }

    AssimpAsset::~AssimpAsset() {
        YZDEBUG("Releasing Assimp Asset: '{}'", m_Path);
    }

    std::string AssimpAsset::getName() const {
        return m_Name;
    }

    std::string AssimpAsset::getPath() const {
        return m_Path;
    }

    std::string AssimpAsset::getDirectory() const {
        return m_Directory;
    }

    std::string AssimpAsset::getExtension() const {
        return m_Extension;
    }

    const aiScene *AssimpAsset::getScene() const {
        return m_Scene.get();
    }

    void releaseAssimpScene(AssimpAsset *asset) {
        if (asset == nullptr) return;
        aiReleaseImport(asset->getScene());
        delete asset;
    }

    std::optional<std::shared_ptr<AssimpAsset>> AssimpAsset::FromPath(const std::string &path, AssimpPostProcess postProcess) {
        if (!fs::exists(path)) {
            YZERROR("Failed to find an Assimp Asset from path: '{}'", path);
            return std::nullopt;
        }

        const aiScene *scene = aiImportFile(path.c_str(), aiProcess_Triangulate | aiProcess_MakeLeftHanded | aiProcess_JoinIdenticalVertices | (uint32_t)postProcess);
        if (scene == nullptr && scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr) {
            YZERROR("Failed to load an Assimp Asset from path: '{}' because of '{}'", path, aiGetErrorString());
            return std::nullopt;
        }

        return std::shared_ptr<AssimpAsset>(new AssimpAsset(path, scene), releaseAssimpScene);
    }
}  // namespace Yulduz
