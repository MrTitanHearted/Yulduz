#pragma once

#include <Yulduz/Core.hpp>
#include <Yulduz/Assets.hpp>
#include <Yulduz/Window.hpp>
#include <Yulduz/Render.hpp>

#if defined(YULDUZ_PLATFORM_WINDOWS) && defined(YULDUZ_BUILD_TYPE_RELEASE)
#define YULDUZ_MAIN() WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
#else
#define YULDUZ_MAIN() main(int argc, char **argv)
#endif

namespace Yulduz {
    void TestYulduzEngine();
}  // namespace Yulduz
