#pragma once

#include <Yulduz/Core.hpp>
#include <YulduzAssets/IAsset.hpp>
#include <YulduzAssets/Enums.hpp>

namespace Yulduz {
    class AssimpAsset : public IAsset {
       public:
        std::optional<std::shared_ptr<AssimpAsset>> FromPath(const std::string &path, AssimpPostProcess postProcess = AssimpPostProcess::None);

       public:
        AssimpAsset(const std::string &path, const aiScene *scene);
        ~AssimpAsset();

        std::string getName() const;
        std::string getPath() const;
        std::string getDirectory() const;
        std::string getExtension() const;
        const aiScene *getScene() const;

        inline const aiScene *operator*() const { return m_Scene.get(); }
        inline operator const aiScene *() const { return m_Scene.get(); }

       private:
        std::string m_Name;
        std::string m_Path;
        std::string m_Directory;
        std::string m_Extension;
        std::unique_ptr<aiScene> m_Scene;
    };
}  // namespace Yulduz
