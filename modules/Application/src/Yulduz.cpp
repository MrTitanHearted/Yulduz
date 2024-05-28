#include <Yulduz/Yulduz.hpp>

#include <Yulduz/Core.hpp>
#include <Yulduz/Assets.hpp>
#include <Yulduz/Window.hpp>
#include <Yulduz/Render.hpp>
#include <Yulduz/Engine.hpp>

namespace Yulduz {
    void TestYulduz() {
        TestYulduzCore();
        TestYulduzAssets();
        TestYulduzWindow();
        TestYulduzRender();
        TestYulduzEngine();
        std::cout << "Hello, " << __FUNCTION__ << "!" << std::endl;
    }
}  // namespace Yulduz
