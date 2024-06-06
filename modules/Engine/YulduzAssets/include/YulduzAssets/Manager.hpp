#pragma once

#include <Yulduz/Core.hpp>
#include <YulduzAssets/IAsset.hpp>

namespace Yulduz {
    class AssetManager {
       public:
        AssetManager(const std::string &label);
        ~AssetManager();

        inline AssetManager() : AssetManager("Yulduz Asset Manager") {}

        inline std::string getLabel() const { return m_Label; }

        template <typename Asset, typename = std::enable_if_t<std::is_base_of<IAsset, Asset>::value>>
        bool hasAsset(const std::string &name) const {
            const std::string &type = typeid(Asset).name();
            return hasAsset(type, name);
        }

        template <typename Asset, typename = std::enable_if_t<std::is_base_of<IAsset, Asset>::value>>
        bool addAsset(const std::string &name, const std::shared_ptr<Asset> &asset) {
            const std::string &type = typeid(Asset).name();
            return addAsset(type, name, asset);
        }

        template <typename Asset, typename = std::enable_if_t<std::is_base_of<IAsset, Asset>::value>>
        bool setAsset(const std::string &name, const std::shared_ptr<Asset> &asset) {
            const std::string &type = typeid(Asset).name();
            return setAsset(type, name, asset);
        }

        template <typename Asset, typename = std::enable_if_t<std::is_base_of<IAsset, Asset>::value>>
        bool removeAsset(const std::string &name) {
            const std::string &type = typeid(Asset).name();
            return removeAsset(type, name);
        }

        template <typename Asset, typename = std::enable_if_t<std::is_base_of<IAsset, Asset>::value>>
        std::optional<std::shared_ptr<Asset>> getAsset(const std::string &name) const {
            const std::string &type = typeid(Asset).name();
            if (!hasAsset(type, name)) return std::nullopt;
            return std::dynamic_pointer_cast<Asset>(getAsset(type, name));
        }

       private:
        using AssetLibrary = std::map<std::string, std::shared_ptr<IAsset>>;

        std::string m_Label;
        std::unordered_map<std::string, AssetLibrary> m_AssetsMap;

       private:
        bool hasAsset(const std::string &type, const std::string &name) const;
        bool addAsset(const std::string &type, const std::string &name, const std::shared_ptr<IAsset> &iasset);
        bool setAsset(const std::string &type, const std::string &name, const std::shared_ptr<IAsset> &iasset);
        bool removeAsset(const std::string &type, const std::string &name);
        std::shared_ptr<IAsset> getAsset(const std::string &type, const std::string &name) const;
    };
}  // namespace Yulduz
