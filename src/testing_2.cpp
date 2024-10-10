#include <iostream>
#include <windows.h>
#include <string>
#include <cstdlib>
#include <opencv2/opencv.hpp>
#include "../include/ScreenGrabber.h"
#include "../include/GameInputHandler.h"
#include "../include/BaseProcessor.h"
#include "../include/utils.h"

int main() {
    // ========= ScreenGrabber setup =========
    ScreenGrabber screen_grabber;
    LPCSTR window_title = "Clash of Clans";
    if (!screen_grabber.initialiseGrabber(window_title)) {
        std::cout << "Failed to initialise grabber" << std::endl;
        return -1;
    }

    // Capture via a simple PrintWindow function 
    // WORKS for CoC; tracks the window, AND ignores overlapping windows 
    HBITMAP hbm_fullscreen = screen_grabber.grabPrintWindow();
    if (!hbm_fullscreen) {
        DeleteObject(hbm_fullscreen);
        std::cout << "HBITMAP of value nullptr deleted." << std::endl;
    }

    // Save the captured bitmap
    std::string bmp_filename = "../bitmaps/AS_bmp_";
    bmp_filename += getCurrentTimestamp();
    bmp_filename += ".bmp";
    std::cout << bmp_filename << std::endl;
    if (screen_grabber.saveScreenGrab(hbm_fullscreen, bmp_filename)) {
        std::cout << "Bitmap successfully saved. Deleting HBITMAP object now." << std::endl;
    } else {
        std::cout << "Saving bitmap failed. Manually deleting HBITMAP obj." << std::endl;
        DeleteObject(hbm_fullscreen);
    }

    //========= BaseProcessor setup =========
    BaseProcessor base_processor;
    if (!base_processor.updateCurrentBitmap(bmp_filename)) {
        std::cout << "Main: Failed to update bmp in base processor." << std::endl;
        return -1;
    }

    // ========= GameInputHandler setup =========
    GameInputHandler in_handler;
    if (!in_handler.initialiseInputHandler(window_title)) {
        std::cout << "Failed to initialse game input handler obj..." << std::endl;
        return -1;
    }
    if (!in_handler.tryClickHUD("NEXT")) {
        std::cout << "Failed to click HUD object..." << std::endl;
        return -1;
    }

    

    base_processor.setSuitableMetricRanges(500000, 500000);

    std::cout << "Trying to read all metric regions..." << std::endl;
    if (!base_processor.readAllMetrics()) {
        std::cout << "readAllMetrics failed :(" << std::endl;
        return -1;
    }
    if (!base_processor.isBaseSuitable()) {
        std::cout << "BASE NOT SUITABLE!" << std::endl;
    } else {
        std::cout << "BASE SUITABLE! YAY" << std::endl;
    }

    bool found_suitable_base = false;
    while (!found_suitable_base) {
        // Take and save new screengrab
        // Update bmp in BaseProcessor
        // Read All Metrics
        //   if false, skip this base -- means some metrics cldnt be read
        // Check base suitability (isBaseSuitable())
        //   if true -- set loop flag to true!
        //   else, use gameInputHandler to click next
        
    }

    return 0;
}