#pragma once

#ifdef _WIN64 
#define GAME_PLATFORM_WINDOWS_64
#define GAME_PTR_SIZE 8
#elif _WIN32
#define GAME_PLATFORM_WINDOWS_32
#define GAME_PTR_SIZE 4
#endif