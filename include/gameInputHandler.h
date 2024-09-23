#include <iostream>
#include <windows.h>

class gameInputHandler {
    HWND hwnd;

public:
    gameInputHandler();
    ~gameInputHandler();

    bool initialiseInputHandler(LPCSTR window_title);

    HWND getHWND();

    bool tryClickHUD(std::string hud_object);
};