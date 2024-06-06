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

}  // namespace Yulduz
