#include <iostream>
#include <windows.h>
#include <unordered_map>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "../include/GameInputHandler.h"



GameInputHandler::GameInputHandler() {
    // Each value is a percentage of the way along the screen in a given direction. Value 1 and 2 are for X; value 3 and 4 are for Y.
    // For a given axis, the two values together mark the bounds in the screen of that HUD object, as a percentage (N/100)
    HUD_object_placements = {
        {"NEXT", {88, 96, 72, 79}}  // The 'next' button is bound by 88% and 96% along on the X-axis, for example.
    };
}
GameInputHandler::~GameInputHandler() {}

bool GameInputHandler::initialiseInputHandler(LPCSTR window_title) {
    hwnd = FindWindowA(NULL, window_title);
    //hwnd = (HWND)0x000806C0; // using handle directly, for testing
    if (!hwnd) {
        std::cout << "Window not found!" << std::endl;
        return false;
    }
    std::cout << "Window successfully found." << std::endl;
    return true;
}

HWND GameInputHandler::getHWND() {
    return hwnd;
}

bool GameInputHandler::tryClickHUD(std::string hud_object) {
    
    RECT client_rect;
    if (!GetClientRect(hwnd, &client_rect)) {
        std::cout << "Failed to get client rectangle." << std::endl;
        return false;
    }

    int width = client_rect.right - client_rect.left;
    int height = client_rect.bottom - client_rect.top;
    // Getting the bounds of the HUD object:
    int hud_obj_lower_x = HUD_object_placements[hud_object][0];
    int hud_obj_upper_x = HUD_object_placements[hud_object][1];
    int hud_obj_lower_y = HUD_object_placements[hud_object][2];
    int hud_obj_upper_y = HUD_object_placements[hud_object][3];
    // Initialising the input coordinate bounds:
    int input_lower_x = width * hud_obj_lower_x / 100;
    int input_upper_x = width * hud_obj_upper_x / 100;
    int input_lower_y = height * hud_obj_lower_y / 100;
    int input_upper_y = height * hud_obj_upper_y / 100;
    // Getting a random coordinate within those bounds:
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    int input_x = input_lower_x + std::rand() % (input_upper_x - input_lower_x + 1);
    int input_y = input_lower_y + std::rand() % (input_upper_y - input_lower_y + 1);
    
    std::cout << "Width: " << width << ",  Height: " << height << std::endl;
    std::cout << "Random x: " << input_x << ",  Random y: " << input_y << std::endl;

    // Click...
    LPARAM l_param = MAKELPARAM(input_x, input_y);
    SendMessage(hwnd, WM_LBUTTONDOWN, MK_LBUTTON, l_param);
    SendMessage(hwnd, WM_LBUTTONUP, 0, l_param);

    return true;
}