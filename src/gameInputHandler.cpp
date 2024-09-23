#include <iostream>
#include <windows.h>
#include <unordered_map>
#include <vector>
#include "../include/gameInputHandler.h"



gameInputHandler::gameInputHandler() {}
gameInputHandler::~gameInputHandler() {}

bool gameInputHandler::initialiseInputHandler(LPCSTR window_title) {
    hwnd = FindWindowA(NULL, window_title);
    //hwnd = (HWND)0x000806C0; // using handle directly, for testing
    if (!hwnd) {
        std::cout << "Window not found!" << std::endl;
        return false;
    }
    std::cout << "Window successfully found." << std::endl;
    return true;
}

HWND gameInputHandler::getHWND() {
    return hwnd;
}

bool gameInputHandler::tryClickHUD(std::string hud_object) {
    // Each value is a percentage of the way along the screen in a given direction. Value 1 and 2 are for X; value 3 and 4 are for Y.
    // For a given axis, the two values together mark the bounds in the screen of that HUD object, as a percentage (N/100)
    std::unordered_map<std::string, std::vector<int>> HUD_object_placements = {
        {"NEXT", {78, 85, 89, 97}}
    };
    RECT client_rect;
    if (!GetClientRect(hwnd, &client_rect)) {
        std::cout << "Failed to get client rectangle." << std::endl;
        return -1;
    }
    int width = client_rect.right - client_rect.left;
    int height = client_rect.bottom - client_rect.top;

    int x = width / 2;
    int y = height / 2;
    std::cout << std::endl << "Width: " << width << ";   Height: " << height << std::endl << "X: " << x << ";   y: " << y;
    LPARAM lParam = MAKELPARAM(x, y);
    SendMessage(hwnd, WM_LBUTTONDOWN, MK_LBUTTON, lParam);
    SendMessage(hwnd, WM_LBUTTONUP, 0, lParam);
}