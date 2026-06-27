#ifndef HELPER_H
#define HELPER_H

#include <string>

namespace StockMarket {
namespace Utils {

    // Returns current date/time as a string in YYYY-MM-DD HH:MM:SS format
    std::string getCurrentTimestamp();

    // Converts a string to uppercase (useful for standardizing ticker codes)
    std::string toUpperCase(const std::string& str);

    // Formats a double to currency format e.g. "$1,234.56"
    std::string formatCurrency(double val);

    // Formats a double to percentage format e.g. "+5.20%" or "-1.45%"
    std::string formatPercentage(double val);

    // Helper to print standard console headers
    void printHeader(const std::string& title);

    // Platform-independent console screen clearing helper
    void clearConsole();

} // namespace Utils
} // namespace StockMarket

#endif // HELPER_H
