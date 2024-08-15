#include <Yulduz/Yulduz.hpp>

int main(int argc, char **argv) {
    Yulduz::Logger::SetLogLevel(Yulduz::LogLevel::Debug);

    Yulduz::App::Run();

    // #if defined(YULDUZ_BUILD_TYPE_DEBUG)
    //     LOG_INFO("Press Enter to exit...");
    //     std::cin.get();
    // #endif

    return 0;
}

#if defined(YULDUZ_BUILD_TYPE_RELEASE) && defined(YULDUZ_PLATFORM_WINDOWS)
#include <Windows.h>

LPSTR ConvertLPWSTRtoLPSTR(LPWSTR lpwstr) {
    int size = WideCharToMultiByte(CP_ACP, 0, lpwstr, -1, NULL, 0, NULL, NULL);
    if (size == 0) {
        return nullptr;
    }
    char *str = new char[size];
    if (WideCharToMultiByte(CP_ACP, 0, lpwstr, -1, str, size, NULL, NULL) == 0) {
        delete[] str;
        return NULL;
    }
    return str;
}

LPSTR *CommandLineToArgvA(int *pArgc) {
    LPWSTR *argvW = CommandLineToArgvW(GetCommandLineW(), pArgc);
    if (argvW == nullptr) {
        return nullptr;
    }
    LPSTR *argvA = new LPSTR[*pArgc];
    for (int i = 0; i < *pArgc; i++) {
        argvA[i] = ConvertLPWSTRtoLPSTR(argvW[i]);
    }
    LocalFree(argvW);
    return argvA;
}

INT WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    LPSTR *argv;
    int argc;
    argv = CommandLineToArgvA(&argc);

    if (argc == 0) {
        return 0;
    }

    int result = main(argc, argv);

    for (int i = 0; i < argc; i++) {
        delete[] argv[i];
    }
    delete[] argv;

    return result;
}
#endif