#include <YulduzAssets/AssetManager.hpp>

namespace Yulduz {
    AssetManager AssetManager::g_AssetManager{};

    AssetManager::AssetManager(const std::string &label)
        : m_Label(label) {
        YZDEBUG("Initializing Asset Manager: '{}'", m_Label);
    }

    AssetManager::~AssetManager() {
        m_TypeToAssetMap.clear();
        YZDEBUG("Releasing Asset Manager: '{}'", m_Label);
    }

    bool AssetManager::hasAsset(const std::string &type, const std::string &name) const {
        if (m_TypeToAssetMap.find(type) == m_TypeToAssetMap.end()) return false;
        const AssetMap &map = m_TypeToAssetMap.at(type);
        return map.find(name) != map.end();
    }

    bool AssetManager::addAsset(const std::string &type, const std::string &name, const IAssetHandle &iasset) {
        AssetMap &map = m_TypeToAssetMap[type];
        if (map.find(name) != map.end()) {
#if defined(YULDUZ_BUILD_TYPE_DEBUG)
            YZWARN("Asset type of '{}' with name '{}' already exists in Asset Manager: '{}'", type, name, m_Label);
#endif
            return false;
        }
        map[name] = iasset;
        return true;
    }

    bool AssetManager::setAsset(const std::string &type, const std::string &name, const IAssetHandle &iasset) {
        AssetMap &map = m_TypeToAssetMap[type];
        map[name] = iasset;
        return true;
    }

    bool AssetManager::removeAsset(const std::string &type, const std::string &name) {
        if (m_TypeToAssetMap.find(type) == m_TypeToAssetMap.end()) return false;
        AssetMap &map = m_TypeToAssetMap[type];
        if (map.find(name) == map.end()) return false;

#if defined(YULDUZ_BUILD_TYPE_DEBUG)
        std::size_t useCount = map[name].use_count() - 1;
        if (useCount > 0) {
            const char *useCountStr = useCount == 1 ? "case" : "cases";
            YZINFO("Asset type of '{}' with name '{}' has been removed from Asset Manager: '{}'. The asset has {} use {} left",
                   type, name, m_Label, useCount, useCountStr);
        }
#endif

        map.erase(name);
        return true;
    }

    AssetManager::IAssetHandle AssetManager::getAsset(const std::string &type, const std::string &name) const {
        return m_TypeToAssetMap.at(type).at(name);
    }

    AssetManager &AssetManager::GetDefault() {
        return g_AssetManager;
    }
}  // namespace Yulduz
