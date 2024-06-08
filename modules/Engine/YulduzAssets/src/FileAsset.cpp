#include <YulduzAssets/FileAsset.hpp>

namespace Yulduz {
    FileAsset::FileAsset(const std::string &path, const std::string &content) {
        YZDEBUG("Initializing File Asset: '{}'", path);
        m_Path = path;
        m_Content = content;
        fs::path fpath(path);
        m_Name = fpath.filename().string();
        m_Directory = fpath.parent_path().string();
        m_Extension = fpath.extension().string();
    }

    FileAsset::~FileAsset() {
        YZDEBUG("Releasing File Asset: '{}'", m_Path);
    }

    std::string FileAsset::getName() const { return m_Name; }

    std::string FileAsset::getPath() const { return m_Path; }

    std::string FileAsset::getDirectory() const { return m_Directory; }

    std::string FileAsset::getExtension() const { return m_Extension; }

    std::string FileAsset::getContent() const { return m_Content; }

    std::shared_ptr<FileAsset> FileAsset::FromContent(const std::string &path, const std::string &content) {
        return std::make_shared<FileAsset>(path, content);
    }

    std::optional<std::shared_ptr<FileAsset>> FileAsset::FromPath(const std::string &path) {
        if (!fs::exists(path)) {
            YZERROR("Failed to find a File Asset from path: '{}'", path);
            return std::nullopt;
        }
        std::ifstream file(path);
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

        return std::make_shared<FileAsset>(path, content);
    }
}  // namespace Yulduz