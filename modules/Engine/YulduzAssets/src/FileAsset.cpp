#include <YulduzAssets/FileAsset.hpp>

namespace Yulduz {
    FileAsset::FileAsset(const fs::path &path, const std::string &content)
        : m_Path(path), m_Content(content) {
        YZDEBUG("Initializing File Asset: '{}'", m_Path.string());
        
        m_Directory = m_Path.parent_path();
        m_Name = m_Path.filename().string();
        m_Extension = m_Path.extension().string();
    }

    FileAsset::~FileAsset() {
        YZDEBUG("Releasing File Asset: '{}'", m_Path.string());
    }

    fs::path FileAsset::getPath() const { return m_Path; }

    fs::path FileAsset::getDirectory() const { return m_Directory; }

    std::string FileAsset::getName() const { return m_Name; }

    std::string FileAsset::getExtension() const { return m_Extension; }

    std::string FileAsset::getContent() const { return m_Content; }

    std::shared_ptr<FileAsset> FileAsset::FromContent(const fs::path &path, const std::string &content) {
        return std::make_shared<FileAsset>(path, content);
    }

    std::optional<std::shared_ptr<FileAsset>> FileAsset::FromPath(const fs::path &path) {
        if (!fs::exists(path)) {
            YZERROR("Failed to find a File Asset from path: '{}'", path.string());
            return std::nullopt;
        }
        std::ifstream ifile{path};
        std::string content{(std::istreambuf_iterator<char>(ifile)), std::istreambuf_iterator<char>()};
        return std::make_shared<FileAsset>(path, content);
    }
}  // namespace Yulduz
