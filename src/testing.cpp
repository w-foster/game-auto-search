#include <iostream>
#include <windows.h>
#include "../include/screengrabber.h"

int main() {
    ScreenGrabber screen_grabber;
    LPCSTR window_title = "Fan Control V157 (userConfig.json)"; // Using a random window I had open for testing
    if (!screen_grabber.initialiseGrabber(window_title)) {
        std::cout << "Failed to initialise grabber" << std::endl;
        return -1;
    }

    HBITMAP hbm_fullscreen = screen_grabber.grabScreen();
    if (!hbm_fullscreen) {
        DeleteObject(hbm_fullscreen);
        std::cout << "HBITMAP of value nullptr deleted." << std::endl;
    }

    if (screen_grabber.saveScreenGrab(hbm_fullscreen, "test_screenshot2.bmp")) {
        std::cout << "Bitmap successfully saved. Deleting HBITMAP object now." << std::endl;
        DeleteObject(hbm_fullscreen);
    }

    return 0;
}