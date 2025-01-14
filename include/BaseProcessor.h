#include <iostream>
#include <opencv2/opencv.hpp>
#include <windows.h>
#include <unordered_map>
#include <utility>

/*
struct BaseInfo {
    int gold, elixir, dark;
    int trophies_win, trophies_lose;

    BaseInfo() {}
};
*/

class BaseProcessor {

    cv::Mat curr_bmp; // stores current bitmap as an OpenCV Matrix
    const std::unordered_map<std::string, std::vector<double>> regions;
    const std::vector<std::string> ordered_region_keys;
    const std::unordered_map<std::string, std::pair<int, int>> possible_metric_ranges;
    std::unordered_map<std::string, int> suitable_metric_ranges;
    //BaseInfo base_info; // Just using a map instead currently
    std::unordered_map<std::string, int> base_metrics;

public:
    BaseProcessor();
    ~BaseProcessor();

    bool updateCurrentBitmap(const std::string &new_bmp_path);
    bool preprocessBitmap(std::string transformation_type);

    void setSuitableMetricRanges(int min_gold=0, int min_elixir=0, int min_dark=0, int min_trophies_win=0, int max_trophies_lose=59);

    bool readRegion(std::string region_name);
    bool readAllMetrics();
    int processMetricText(std::string region_name, std::string &text);

    bool isBaseSuitable();

};
