#ifndef TEMPLATES_H
#define TEMPLATES_H

#include <iostream>
#include <limits>
#include <string>

namespace StockMarket {

    /**
     * @brief Template function to prompt user and receive validated numeric inputs.
     * Prevents infinite loops caused by invalid stream state and validates range boundaries.
     */
    template <typename T>
    T getValidInput(const std::string& prompt, 
                    T minVal = std::numeric_limits<T>::lowest(), 
                    T maxVal = std::numeric_limits<T>::max()) {
        T value;
        while (true) {
            std::cout << prompt;
            if (std::cin >> value) {
                // Input succeeded, verify bounds
                if (value >= minVal && value <= maxVal) {
                    // Clear the buffer of trailing characters (including newline)
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    return value;
                }
                std::cout << "Error: Input must be in range [" << minVal << ", " << maxVal << "]. Please try again.\n";
            } else {
                // Input failed (wrong type provided)
                std::cout << "Error: Invalid input type. Please enter a valid number.\n";
                std::cin.clear(); // Clear the error flags
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard bad characters
            }
        }
    }

} // namespace StockMarket

#endif // TEMPLATES_H
