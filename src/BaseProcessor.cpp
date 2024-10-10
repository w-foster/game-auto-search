#include <iostream>
#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include <windows.h>
#include <unordered_map>
#include <cstdlib>
#include "../include/BaseProcessor.h"


// Constructor and destructor
BaseProcessor::BaseProcessor() 
: curr_bmp(), base_info() {
    regions = {
        {"GOLD TOTAL", {4.5, 12, 10.25, 15}}, 
        {"ELIXIR TOTAL", {4.5, 12, 15, 20}}, 
        {"DARK TOTAL", {4.5, 12, 20, 24}}, 
        {"TROPHIES WIN", {4.5, 8, 26, 31}}, 
        {"TROPHIES LOSE", {4.5, 8, 34, 39}}
    };

    std::vector<std::string> ordered_region_keys = {
        "GOLD TOTAL", 
        "ELIXIR TOTAL", 
        "DARK TOTAL", 
        "TROPHIES WIN", 
        "TROPHIES LOSE"
    };
}

BaseProcessor::~BaseProcessor() {}

// Other methods
bool BaseProcessor::updateCurrentBitmap(const std::string &new_bmp_path) {
    curr_bmp = cv::imread(new_bmp_path, cv::IMREAD_COLOR);

    if (curr_bmp.empty()) {
        std::cout << "BaseProcessor: Failed to load image into OpenCV Matrix." << std::endl;
        return false;
    }
    std::cout << "BaseProcessor: Image successfully loaded into OpenCV Matrix." << std::endl;

    //preprocessBitmap("GRAYSCALE");
    //preprocessBitmap("CONTRAST");

    cv::imshow("Current Bmp", curr_bmp);
    cv::waitKey(5000);
    cv::destroyWindow("Current Bmp");
    return true;
}

bool BaseProcessor::preprocessBitmap(std::string transformation_type) {
    if (transformation_type == "GRAYSCALE") {
        cv::cvtColor(curr_bmp, curr_bmp, cv::COLOR_BGR2GRAY);
    }
    if (transformation_type == "CONTRAST") {
        curr_bmp.convertTo(curr_bmp, -1, 1.0, 0);
    }
    return true;
}

bool BaseProcessor::readRegion(std::string region_name) {
    std::vector<double> region_bounds = regions[region_name];
    for (int i = 0; i < 4; i++) {
        std::cout << region_bounds[i] << "%  ";
    }
    std::cout << std::endl;

    int bmp_width = curr_bmp.cols;
    int bmp_height = curr_bmp.rows;

    int x = static_cast<int>((bmp_width * region_bounds[0]) / 100);
    int y = static_cast<int>((bmp_height * region_bounds[2]) / 100);
    int width = static_cast<int>((bmp_width * region_bounds[1]) / 100 - x);
    int height = static_cast<int>((bmp_height * region_bounds[3]) / 100 - y);

    std::cout << "BaseProcessor coords: " << std::endl;
    std::cout << "bmp_width: " << bmp_width << ";  bmp_height: " << bmp_height << std::endl;
    std::cout << "x: " << x << ";  y: " << y << ";  width: " << width << ";  height: " << height << std::endl;

    cv::Rect region_rect(x, y, width, height);
    // Create a REFERENCE to the region of interest within the bitmap:
    cv::Mat region = curr_bmp(region_rect);

    cv::imshow("ROI", region);
    cv::waitKey(10000);
    cv::destroyWindow("ROI");

    // Initialise Tesseract API
    tesseract::TessBaseAPI ocr;
    if (ocr.Init("C:/msys64/mingw64/share/tessdata", "eng", tesseract::OEM_LSTM_ONLY)) {
        std::cout << "BaseProcessor: Failed to initialise Tesseract API." << std::endl;
        return false;
    }

    ocr.SetImage(region.data, region.cols, region.rows, 3, region.step);

    char* text = ocr.GetUTF8Text();
    std::cout << "===" << region_name << "===" << std::endl;
    std::cout << "Recognised Text: " << text << std::endl;
    text = ocr.GetUTF8Text();

    // Cleanup:
    ocr.End();
    delete[] text;

    return true;
}

bool BaseProcessor::readAllMetrics() {
    for (const auto& pair : regions) {
        if (!readRegion(pair.first)) {
            std::cout << "BaseProcessor: readAllMetrics: reading a region failed." << std::endl;
            return false;
        }
    }
    /*
    Alternative method, reading them in the order defined in the ordered_region_keys vector attribute:
    for (const std::string& region_name : ordered_region_keys) {
        if (!readRegion(regions[region_name])) {
            std::cout << "BaseProcessor: readAllMetrics: reading a region failed." << std::endl;
            return false;
        }
    }
    */
    return true;
}


