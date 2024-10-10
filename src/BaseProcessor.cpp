#include <iostream>
#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include <windows.h>
#include <unordered_map>
#include "../include/BaseProcessor.h"

// Constructor and destructor
BaseProcessor::BaseProcessor() 
: curr_bmp(), base_info() {
    regions = {
        {"GOLD TOTAL", {4, 12, 10, 15}}, 
        {"ELIXIR TOTAL", {4, 12, 15, 20}}, 
        {"DARK TOTAL", {4, 12, 20, 24}}, 
        {"TROPHIES WIN", {4, 8, 26, 31}}, 
        {"TROPHIES LOSE", {4, 8, 34, 39}}
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

    cv::imshow("Current Bmp", curr_bmp);
    cv::waitKey(5000);
    cv::destroyWindow("Current Bmp");
}

bool BaseProcessor::cropBitmap(std::string region_name) {
    std::vector<int> &region_bounds = regions[region_name];
    int x = region_bounds[0], y = region_bounds[2];
    int width = x - region_bounds[1], height = y - region_bounds[3];

    cv::Rect region_rect(x, y, width, height);
    // Create a REFERENCE to the region of interest within the bitmap:
    cv::Mat region = curr_bmp(region_rect);

    // Initialise Tesseract API
    tesseract::TessBaseAPI ocr;
    if (ocr.Init(NULL, "eng", tesseract::OEM_LSTM_ONLY)) {
        std::cout << "Failed to initialise Tesseract API." << std::endl;
        return false;
    }

    ocr.SetImage(region.data, region.cols, region.rows, 3, region.step);

    char* text = ocr.GetUTF8Text();
    std::cout << "Recognised Text: " << text << std::endl;

    // Cleanup:
    ocr.End();
    delete[] text;

    return true;
}


