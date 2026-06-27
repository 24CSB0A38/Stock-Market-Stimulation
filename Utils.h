/*
 * ============================================================================
 *  Utils.h — Exceptions, Helper Functions, and Template Utilities
 * ============================================================================
 *  Contains all utility code for the project in a single header:
 *    - Custom Exception Hierarchy (6 domain-specific exceptions)
 *    - Helper Functions (formatting, timestamps, console utilities)
 *    - Template Input Validator (type-safe numeric input)
 * ============================================================================
 */

#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <limits>

using namespace std;

// =============================================================================
// CUSTOM EXCEPTION HIERARCHY
// =============================================================================

// Base exception for all domain-specific errors
class MarketSystemException : public runtime_error {
public:
    explicit MarketSystemException(const string& message) : runtime_error(message) {}
};

// Thrown when user has insufficient balance to execute buy orders
class InsufficientBalanceException : public MarketSystemException {
public:
    explicit InsufficientBalanceException(const string& message)
        : MarketSystemException("InsufficientBalanceException: " + message) {}
};

// Thrown when an operation references a stock ticker not loaded in market
class StockNotFoundException : public MarketSystemException {
public:
    explicit StockNotFoundException(const string& message)
        : MarketSystemException("StockNotFoundException: " + message) {}
};

// Thrown during registration if the requested username is already taken
class DuplicateUserException : public MarketSystemException {
public:
    explicit DuplicateUserException(const string& message)
        : MarketSystemException("DuplicateUserException: " + message) {}
};

// Thrown during login if password mismatch or username does not exist
class AuthenticationFailedException : public MarketSystemException {
public:
    explicit AuthenticationFailedException(const string& message)
        : MarketSystemException("AuthenticationFailedException: " + message) {}
};

// Thrown if shares volume ordered is negative, zero or exceeds inventory
class InvalidShareQuantityException : public MarketSystemException {
public:
    explicit InvalidShareQuantityException(const string& message)
        : MarketSystemException("InvalidShareQuantityException: " + message) {}
};

// Thrown when validation fails (e.g. negative prices, invalid tickers)
class InvalidStockException : public MarketSystemException {
public:
    explicit InvalidStockException(const string& message)
        : MarketSystemException("InvalidStockException: " + message) {}
};

// =============================================================================
// HELPER FUNCTIONS
// =============================================================================

namespace Utils {

    // Returns current date/time as "YYYY-MM-DD HH:MM:SS"
    inline string getCurrentTimestamp() {
        auto now = chrono::system_clock::now();
        time_t now_time = chrono::system_clock::to_time_t(now);
        tm tm_struct;
#if defined(_WIN32) || defined(_WIN64)
        localtime_s(&tm_struct, &now_time);
#else
        localtime_r(&now_time, &tm_struct);
#endif
        ostringstream oss;
        oss << put_time(&tm_struct, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }

    // Converts a string to uppercase
    inline string toUpperCase(const string& str) {
        string upperStr = str;
        transform(upperStr.begin(), upperStr.end(), upperStr.begin(),
                  [](unsigned char c) { return toupper(c); });
        return upperStr;
    }

    // Formats a double to currency e.g. "$1234.56"
    inline string formatCurrency(double val) {
        ostringstream oss;
        oss << fixed << setprecision(2);
        if (val < 0) {
            oss << "-$" << -val;
        } else {
            oss << "$" << val;
        }
        return oss.str();
    }

    // Formats a double to percentage e.g. "+5.20%"
    inline string formatPercentage(double val) {
        ostringstream oss;
        oss << fixed << setprecision(2);
        if (val > 0) oss << "+";
        oss << val << "%";
        return oss.str();
    }

    // Prints a standard console section header
    inline void printHeader(const string& title) {
        cout << "\n==================================================\n";
        cout << "  " << title << "\n";
        cout << "==================================================\n";
    }

    // Clears the console screen (ANSI escape)
    inline void clearConsole() {
        cout << "\033[2J\033[1;1H";
    }

} // namespace Utils

// =============================================================================
// TEMPLATE INPUT VALIDATOR
// =============================================================================

// Generic function for safe, range-validated numeric input
template <typename T>
T getValidInput(const string& prompt,
                T minVal = numeric_limits<T>::lowest(),
                T maxVal = numeric_limits<T>::max()) {
    T value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            if (value >= minVal && value <= maxVal) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return value;
            }
            cout << "Error: Input must be in range [" << minVal << ", " << maxVal << "]. Please try again.\n";
        } else {
            cout << "Error: Invalid input type. Please enter a valid number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

#endif // UTILS_H
