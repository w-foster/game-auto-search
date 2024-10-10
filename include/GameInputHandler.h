#include <iostream>
#include <windows.h>
#include <vector>
#include <unordered_map>

class GameInputHandler {
    HWND hwnd;
    std::unordered_map<std::string, std::vector<int>> HUD_object_placements;

public:
    GameInputHandler();
    ~GameInputHandler();

    bool initialiseInputHandler(LPCSTR window_title);

    HWND getHWND();

    bool tryClickHUD(std::string hud_object);
};