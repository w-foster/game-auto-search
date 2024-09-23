#include <iostream>
#include <windows.h>
#include <string>
#include "../include/screengrabber.h"
#include "../include/utils.h"

int main() {
    ScreenGrabber screen_grabber;
    //LPCSTR window_title = "Fan Control V157 (userConfig.json)"; // Using a random window I had open for testing
    LPCSTR window_title = "Clash of Clans";
    if (!screen_grabber.initialiseGrabber(window_title)) {
        std::cout << "Failed to initialise grabber" << std::endl;
        return -1;
    }

    // === Capture via a client rect, for the window itself ===
    // === FAILS for CoC -- black screen; perhaps anti-cheat? ===
    // HBITMAP hbm_fullscreen = screen_grabber.grabScreen();
    // if (!hbm_fullscreen) {
    //     DeleteObject(hbm_fullscreen);
    //     std::cout << "HBITMAP of value nullptr deleted." << std::endl;
    // }

    // === Capture via desktop ===
    // === FAILS for CoC -- also back screen ===
    // HBITMAP hbm_fullscreen = screen_grabber.grabScreenViaDesktop();
    // if (!hbm_fullscreen) {
    //     DeleteObject(hbm_fullscreen);
    //     std::cout << "HBITMAP of value nullptr deleted." << std::endl;
    // }

    // === Capture via the parent handler ===
    // === FAILS for CoC -- cannot find parent handler ===
    // HBITMAP hbm_fullscreen = screen_grabber.grabScreenViaParent();
    // if (!hbm_fullscreen) {
    //     DeleteObject(hbm_fullscreen);
    //     std::cout << "HBITMAP of value nullptr deleted." << std::endl;
    // }

    // === Capture via a an invisible, tracking overlay ===
    // === WORKS partially - but, overlay is unstable, and overlapping windows disrupt the screengrab ===
    // if (!screen_grabber.createSneakyOverlay()) {
    //     std::cout << "Failed to create sneaky overlay." << std::endl;
    // }
    // HBITMAP hbm_fullscreen = screen_grabber.grabSneakyOverlay();
    // if (!hbm_fullscreen) {
    //     DeleteObject(hbm_fullscreen);
    //     std::cout << "HBITMAP of value nullptr deleted." << std::endl;
    // }

    // === Capture via a simple PrintWindow function ===
    // === WORKS for CoC; tracks the window, AND ignores overlapping windows ===
    HBITMAP hbm_fullscreen = screen_grabber.grabPrintWindow();
    if (!hbm_fullscreen) {
        DeleteObject(hbm_fullscreen);
        std::cout << "HBITMAP of value nullptr deleted." << std::endl;
    }

    
    std::string bmp_filename = "AS_bmp_";
    bmp_filename += getCurrentTimestamp();
    bmp_filename += ".bmp";
    std::cout << bmp_filename << std::endl;
    if (screen_grabber.saveScreenGrab(hbm_fullscreen, bmp_filename)) {
        std::cout << "Bitmap successfully saved. Deleting HBITMAP object now." << std::endl;
    } else {
        std::cout << "Saving bitmap failed. Manually deleting HBITMAP obj." << std::endl;
        DeleteObject(hbm_fullscreen);
    }

    return 0;
}