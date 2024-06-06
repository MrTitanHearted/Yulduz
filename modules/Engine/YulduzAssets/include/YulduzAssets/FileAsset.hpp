#pragma once

#include <Yulduz/Core.hpp>
#include <YulduzAssets/IAsset.hpp>

namespace Yulduz {
    class FileAsset : public IAsset {
       public:
        static std::shared_ptr<FileAsset> FromContent(const std::string &path, const std::string &content);
        static std::optional<std::shared_ptr<FileAsset>> FromPath(const std::string &path);

       public:
        FileAsset(const std::string &path, const std::string &content);
        ~FileAsset();

        std::string getName() const;
        std::string getPath() const;
        std::string getDirectory() const;
        std::string getExtension() const;
        std::string getContent() const;

        inline const char *operator*() const { return m_Content.c_str(); }
        inline operator const std::string &() const { return m_Content; }

       private:
        std::string m_Name;
        std::string m_Path;
        std::string m_Directory;
        std::string m_Extension;
        std::string m_Content;
    };
}  // namespace Yulduz
