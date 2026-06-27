#include "utils/Helper.h"
#include <algorithm>
#include <cctype>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <cstdlib>

namespace StockMarket {
namespace Utils {

    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        std::tm tm_struct;
#if defined(_WIN32) || defined(_WIN64)
        localtime_s(&tm_struct, &now_time);
#else
        localtime_r(&now_time, &tm_struct);
#endif
        std::ostringstream oss;
        oss << std::put_time(&tm_struct, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }

    std::string toUpperCase(const std::string& str) {
        std::string upperStr = str;
        std::transform(upperStr.begin(), upperStr.end(), upperStr.begin(),
                       [](unsigned char c) { return std::toupper(c); });
        return upperStr;
    }

    std::string formatCurrency(double val) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2);
        if (val < 0) {
            oss << "-$" << -val;
        } else {
            oss << "$" << val;
        }
        return oss.str();
    }

    std::string formatPercentage(double val) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2);
        if (val > 0) {
            oss << "+";
        }
        oss << val << "%";
        return oss.str();
    }

    void printHeader(const std::string& title) {
        std::cout << "\n==================================================\n";
        std::cout << "  " << title << "\n";
        std::cout << "==================================================\n";
    }

    void clearConsole() {
        // Simple ANSI escape sequence to clear screen and move cursor to home position.
        // Extremely fast and does not require spawning shell sub-processes.
        std::cout << "\033[2J\033[1;1H";
    }

} // namespace Utils
} // namespace StockMarket
