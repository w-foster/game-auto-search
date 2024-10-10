#include <iostream>
#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include <windows.h>
#include <unordered_map>
#include <cstdlib>
#include <string>
#include "../include/BaseProcessor.h"


// Constructor and destructor
BaseProcessor::BaseProcessor() 
: curr_bmp(), base_metrics(), suitable_metric_ranges(),  
    regions{
        {"GOLD TOTAL", {4.5, 12, 10.25, 15}}, 
        {"ELIXIR TOTAL", {4.5, 12, 15, 20}}, 
        {"DARK TOTAL", {4.5, 12, 20, 24}}, 
        {"TROPHIES WIN", {4.5, 8, 26, 31}}, 
        {"TROPHIES LOSE", {4.5, 8, 34, 39}}
    },

    ordered_region_keys{
        "GOLD TOTAL", 
        "ELIXIR TOTAL", 
        "DARK TOTAL", 
        "TROPHIES WIN", 
        "TROPHIES LOSE"
    },

    possible_metric_ranges{
        {"GOLD TOTAL", {0, 2600000}}, 
        {"ELIXIR TOTAL", {0, 2600000}}, 
        {"DARK TOTAL", {0, 18000}}, 
        {"TROPHIES WIN", {1, 59}}, 
        {"TROPHIES LOSE", {1, 59}}
    }
{}

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

    //cv::imshow("Current Bmp", curr_bmp);
    //cv::waitKey(5000);
    //cv::destroyWindow("Current Bmp");
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

void BaseProcessor::setSuitableMetricRanges(int min_gold, int min_elixir, int min_dark, int min_trophies_win, int max_trophies_lose) {
    suitable_metric_ranges["GOLD TOTAL"] = min_gold;
    suitable_metric_ranges["ELIXIR TOTAL"] = min_elixir;
    suitable_metric_ranges["DARK TOTAL"] = min_dark;
    suitable_metric_ranges["TROPHIES WIN"] = min_trophies_win;
    suitable_metric_ranges["TROPHIES LOSE"] = max_trophies_lose;
}

bool BaseProcessor::readRegion(std::string region_name) {
    std::vector<double> region_bounds = regions.at(region_name);
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

    //cv::imshow("ROI", region);
    //cv::waitKey(10000);
    //cv::destroyWindow("ROI");

    // Initialise Tesseract API
    tesseract::TessBaseAPI ocr;
    if (ocr.Init("C:/msys64/mingw64/share/tessdata", "eng", tesseract::OEM_LSTM_ONLY)) {
        std::cout << "BaseProcessor: Failed to initialise Tesseract API." << std::endl;
        return false;
    }

    ocr.SetImage(region.data, region.cols, region.rows, 3, region.step);

    std::string text = ocr.GetUTF8Text();
    std::cout << "===" << region_name << "===" << std::endl;
    std::cout << "Recognised Text: " << text << std::endl;
    
    int processed_metric = processMetricText(region_name, text);
    std::cout << "Processed (int) metric: " << processed_metric << std::endl;

    // Cleanup:
    ocr.End();

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
    Alternative method: reading them in the order defined in the ordered_region_keys vector attribute:
    for (const std::string& region_name : ordered_region_keys) {
        if (!readRegion(regions[region_name])) {
            std::cout << "BaseProcessor: readAllMetrics: reading a region failed." << std::endl;
            return false;
        }
    }
    */
    return true;
}

int BaseProcessor::processMetricText(std::string region_name, std::string &text) {
    if (text.size() == 0) {
        std::cout << "Empty text... cannot process" << std::endl;
        return -1;
    }

    // Remove non-int chars:
    int i = 0;
    while (i < text.size()) {
        char ch = text[i];
        if (ch < '0' || ch > '9') {
            text.erase(i, 1);
        } else {
            i++;
        }
    }

    std::cout << "Cleaned text: " << text << std::endl;
    if (text.size() == 0) {
        std::cout << "Cleaned text is empty." << std::endl;
        return -1;
    }

    // Check against pre-defined possible ranges:
    int metric = std::stoi(text);
    if (metric < possible_metric_ranges.at(region_name).first || metric > possible_metric_ranges.at(region_name).second) {
        std::cout << "Metric outside of possible bounds." << std::endl;
        return -1;
    }

    base_metrics[region_name] = metric;
    return metric;
}

bool BaseProcessor::isBaseSuitable() {
    // i < size - 1, because we will handle max trophies lose afterwards (separately)
    for (int i = 0; i < ordered_region_keys.size() - 1; i++) {
        std::string metric_name = ordered_region_keys[i];
        if (base_metrics[metric_name] < suitable_metric_ranges[metric_name]) {
            std::cout << "Base not suitable! Metric too low: " << metric_name << std::endl;
            return false;
        }
    }
    std::string trophies_lose = "TROPHIES LOSE";
    if (base_metrics[trophies_lose] > suitable_metric_ranges[trophies_lose]) {
        std::cout << "Base not suitable! Metric too high: " << trophies_lose << std::endl;
    }
    return true;
}
