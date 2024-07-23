#include "CoreTypes.h"

int main() {
    #if defined(_WIN32)
        Editor::WindowsPlatform platform;
    #elif defined(__APPLE__)
        Editor::MacPlatform platform;
    #elif defined(__linux__)
        Editor::LinuxPlatform platform;
    #else
        #error Unsupported platform
    #endif

    platform.CreateWindow(800, 600);
    platform.PumpEvents();

    return 0;
}
