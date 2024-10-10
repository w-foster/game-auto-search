#include <iostream>
#include <opencv2/opencv.hpp>
#include <windows.h>
#include <unordered_map>

struct BaseInfo {
    int gold, elixir, dark;
    int trophies_win, trophies_lose;

    BaseInfo() {}
};

class BaseProcessor {

    /*
    HBITMAP current_bmp
    map regions = {
        {"GOLD AVAILABLE": {x1, x2, y1, y2}}
    }
    BaseInfo base_info;

    updateCurrentBitmap()
    cropBitmap(region name string e.g. "GOLD AVAILABLE")
    preprocessBitmap(bmp, grayscale / high contrast) --> pass e.g. the cropped bitmap
        - This may do, grayscale, or contrast up#
    findText(bmp) --> find the text e.g. 875,125 (amount of Gold)

    higher level methods?
    cropLoot(bmp) -> this might make 3 images, one for gold, elixir, DE each

    NON-IMAGE METHODS:
    checkBaseSuitability(base_info) --> check according to some params
    */

    cv::Mat curr_bmp; // stores current bitmap as an OpenCV Matrix
    std::unordered_map<std::string, std::vector<double>> regions;
    std::vector<std::string> ordered_region_keys;
    BaseInfo base_info;

public:
    BaseProcessor();
    ~BaseProcessor();

    bool updateCurrentBitmap(const std::string &new_bmp_path);
    bool preprocessBitmap(std::string transformation_type);
    bool readRegion(std::string region_name);
    bool readAllMetrics();

};
