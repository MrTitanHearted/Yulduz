#pragma once

#include <Yulduz/Core.hpp>
#include <YulduzAssets/IAsset.hpp>

namespace Yulduz {
    class FileAsset : public IAsset {
       public:
        static std::shared_ptr<FileAsset> FromContent(const fs::path &path, const std::string &content);
        static std::optional<std::shared_ptr<FileAsset>> FromPath(const fs::path &path);

       public:
        FileAsset(const fs::path &path, const std::string &content);
        ~FileAsset();

        fs::path getPath() const;
        fs::path getDirectory() const;
        std::string getName() const;
        std::string getExtension() const;
        std::string getContent() const;

        inline const char *operator*() const { return m_Content.c_str(); }
        inline operator const std::string &() const { return m_Content; }

       private:
        fs::path m_Path;
        fs::path m_Directory;
        std::string m_Name;
        std::string m_Extension;
        std::string m_Content;
    };
}  // namespace Yulduz
