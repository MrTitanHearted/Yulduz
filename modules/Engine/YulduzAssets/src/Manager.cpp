#include <YulduzAssets/Manager.hpp>

namespace Yulduz {
    AssetManager::AssetManager(const std::string &label) {
        YZDEBUG("Initializing Asset Manager: '{}'", label);
        m_Label = label;
    }

    AssetManager::~AssetManager() {
        m_AssetsMap.clear();
        YZDEBUG("Releasing Asset Manager: '{}'", m_Label);
    }

    bool AssetManager::hasAsset(const std::string &type, const std::string &name) const {
        if (m_AssetsMap.find(type) == m_AssetsMap.end()) return false;
        const AssetLibrary &library = m_AssetsMap.at(type);
        return library.find(name) != library.end();
    }

    bool AssetManager::addAsset(const std::string &type, const std::string &name, const std::shared_ptr<IAsset> &iasset) {
        AssetLibrary &library = m_AssetsMap[type];
        if (library.find(name) != library.end()) {
#if defined(YULDUZ_BUILD_TYPE_DEBUG)
            YZWARN("Asset type of '{}' with name '{}' already exists in '{}' Asset Manager", type, name, m_Label);
#endif
            return false;
        }
        library[name] = iasset;
        return true;
    }

    bool AssetManager::setAsset(const std::string &type, const std::string &name, const std::shared_ptr<IAsset> &iasset) {
        AssetLibrary &library = m_AssetsMap[type];
        library[name] = iasset;
        return true;
    }

    bool AssetManager::removeAsset(const std::string &type, const std::string &name) {
        if (m_AssetsMap.find(type) == m_AssetsMap.end()) return false;
        AssetLibrary &library = m_AssetsMap.at(type);
        if (library.find(name) == library.end()) return true;
#if defined(YULDUZ_BUILD_TYPE_DEBUG)
        size_t useCount = library[name].use_count() - 1;
        if (useCount > 0) {
            YZINFO("Asset type of '{}' with name '{}' has been removed from '{}' Asset Manager. It has '{}' use {} left",
                    type, name, m_Label, useCount, useCount == 1 ? "case" : "cases");
        }
#endif
        library.erase(name);
        return true;
    }

    std::shared_ptr<IAsset> AssetManager::getAsset(const std::string &type, const std::string &name) const {
        return m_AssetsMap.at(type).at(name);
    }
}  // namespace Yulduz
