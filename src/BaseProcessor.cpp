#include <iostream>
#include <opencv2/opencv.hpp>
#include <windows.h>
#include <unordered_map>
#include "../include/BaseProcessor.h"

// Constructor and destructor
BaseProcessor::BaseProcessor() 
: curr_bmp(), base_info() {
    regions = {
        {"GOLD AVAILABLE", {5, 50, 15, 20}} // Dummy example
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

