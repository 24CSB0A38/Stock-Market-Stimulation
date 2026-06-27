#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>
#include <string>

namespace StockMarket {

    // Base Exception for all domain-specific errors in the system
    class MarketSystemException : public std::runtime_error {
    public:
        explicit MarketSystemException(const std::string& message) 
            : std::runtime_error(message) {}
    };

    // Thrown when user has insufficient balance to execute buy orders
    class InsufficientBalanceException : public MarketSystemException {
    public:
        explicit InsufficientBalanceException(const std::string& message) 
            : MarketSystemException("InsufficientBalanceException: " + message) {}
    };

    // Thrown when an operations references a stock ticker not loaded in market
    class StockNotFoundException : public MarketSystemException {
    public:
        explicit StockNotFoundException(const std::string& message) 
            : MarketSystemException("StockNotFoundException: " + message) {}
    };

    // Thrown during registration if the requested username is already taken
    class DuplicateUserException : public MarketSystemException {
    public:
        explicit DuplicateUserException(const std::string& message) 
            : MarketSystemException("DuplicateUserException: " + message) {}
    };

    // Thrown during login if password matches or username does not exist
    class AuthenticationFailedException : public MarketSystemException {
    public:
        explicit AuthenticationFailedException(const std::string& message) 
            : MarketSystemException("AuthenticationFailedException: " + message) {}
    };

    // Thrown if the shares volume ordered is negative, zero or exceeds inventory
    class InvalidShareQuantityException : public MarketSystemException {
    public:
        explicit InvalidShareQuantityException(const std::string& message) 
            : MarketSystemException("InvalidShareQuantityException: " + message) {}
    };

    // Thrown when active validation fails (e.g. negative prices, invalid tickers)
    class InvalidStockException : public MarketSystemException {
    public:
        explicit InvalidStockException(const std::string& message) 
            : MarketSystemException("InvalidStockException: " + message) {}
    };

} // namespace StockMarket

#endif // EXCEPTIONS_H
