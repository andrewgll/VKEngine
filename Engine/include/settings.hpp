#pragma once

#if defined(_WIN32) || defined(_WIN64)
#define VKENGINE_ABSOLUTE_PATH \
    (std::string(std::getenv("APPDATA")) + "\\VKEngine\\")
#elif defined(__APPLE__) || defined(__MACH__)
#define VKENGINE_ABSOLUTE_PATH \
    (std::string(std::getenv("HOME")) + "/Library/Application Support/VKEngine/")
#else
#define VKENGINE_ABSOLUTE_PATH "Unsupported platform"
#endif

#define WIDTH 1920
#define HEIGHT 1080

#define MAX_FRAME_TIME 0.1f
#define SHADOWMAP_DIM 2048
