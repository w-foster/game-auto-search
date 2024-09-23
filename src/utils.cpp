#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include "../include/utils.h"

std::string getCurrentTimestamp() {
    // Get current time as time_point
    auto now = std::chrono::system_clock::now();
    
    // Convert to time_t for easy formatting
    std::time_t timeNow = std::chrono::system_clock::to_time_t(now);
    
    // Convert to local time
    std::tm localTime;
    localtime_s(&localTime, &timeNow); // Use localtime_r on Unix systems

    // Format the time as a string
    std::ostringstream oss;
    oss << std::put_time(&localTime, "%Y_%m_%d_t-%H-%M-%S"); // Customize format as needed

    std::string output = oss.str();
    std::cout << output << std::endl;
    return output;
}