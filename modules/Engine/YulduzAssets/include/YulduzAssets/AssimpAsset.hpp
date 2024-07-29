#pragma once

#include <Yulduz/Core.hpp>
#include <YulduzAssets/Enums.hpp>
#include <YulduzAssets/IAsset.hpp>

namespace Yulduz {
    class AssimpAsset : public IAsset {
       public:
        static std::optional<std::shared_ptr<AssimpAsset>> FromPath(const fs::path &path, AssimpPostProcess postProcess = AssimpPostProcess::None);

       public:
        AssimpAsset(const fs::path &path, aiScene *scene);
        ~AssimpAsset();

        fs::path getPath();
        fs::path getDirectory();
        std::string getName();
        std::string getExtension();
        aiScene *getScene();

        inline aiScene *operator*() const { return m_Scene; }
        inline operator aiScene *() const { return m_Scene; }

       private:
        fs::path m_Path;
        fs::path m_Directory;
        std::string m_Name;
        std::string m_Extension;
        aiScene *m_Scene;
    };
}  // namespace Yulduz