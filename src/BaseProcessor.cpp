#include <iostream>
#include <opencv2/opencv.hpp>
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



