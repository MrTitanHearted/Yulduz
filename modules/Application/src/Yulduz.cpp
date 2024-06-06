#include <Yulduz/Yulduz.hpp>

#include <Yulduz/Core.hpp>
#include <Yulduz/Assets.hpp>
#include <Yulduz/Window.hpp>
#include <Yulduz/Render.hpp>
#include <Yulduz/Engine.hpp>

namespace Yulduz {
    void testYulduz() {
        std::shared_ptr<TextureAsset> wallTextureAsset = TextureAsset::FromPath("assets/textures/wall.jpg").value();
        AssetManager assetManager;
        assetManager.addAsset("wall", wallTextureAsset);

        {
            std::optional<std::shared_ptr<TextureAsset>> assetOption = assetManager.getAsset<TextureAsset>("wall");
            if (assetOption) {
                std::shared_ptr<TextureAsset> asset = assetOption.value();
                YZINFO("Texture '{}' from path '{}' {}x{} size={} and channelCount={}",
                       asset->getName(),
                       asset->getPath(),
                       asset->getWidth(),
                       asset->getHeight(),
                       asset->getSize(),
                       asset->getChannelCount());
            }
        }
    }

    void TestYulduz() {
        Logger::Initialize(LogLevel::Info);

        try {
            testYulduz();
        } catch (const std::exception &e) {
            YZFATAL("Program throwed Exception: '{}'", e.what());
        }

        Logger::Shutdown();
    }
}  // namespace Yulduz
