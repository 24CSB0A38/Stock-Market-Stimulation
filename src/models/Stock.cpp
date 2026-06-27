#include "models/Stock.h"
#include "utils/Exceptions.h"

namespace StockMarket {

    Stock::Stock() 
        : ticker(""), companyName(""), currentPrice(0.0), previousPrice(0.0), 
          initialPrice(0.0), totalSharesTraded(0) {}

    Stock::Stock(const std::string& ticker, const std::string& companyName, double price)
        : ticker(ticker), companyName(companyName), currentPrice(price), 
          previousPrice(price), initialPrice(price), totalSharesTraded(0) {
        if (price <= 0.0) {
            throw InvalidStockException("Initial stock price must be greater than zero.");
        }
        priceHistory.push_back(price);
    }

    std::string Stock::getTicker() const { return ticker; }
    std::string Stock::getCompanyName() const { return companyName; }
    double Stock::getCurrentPrice() const { return currentPrice; }
    double Stock::getPreviousPrice() const { return previousPrice; }
    double Stock::getInitialPrice() const { return initialPrice; }
    int Stock::getTotalSharesTraded() const { return totalSharesTraded; }
    const std::vector<double>& Stock::getPriceHistory() const { return priceHistory; }

    double Stock::getPriceChangePercent() const {
        if (previousPrice <= 0.0) return 0.0;
        return ((currentPrice - previousPrice) / previousPrice) * 100.0;
    }

    double Stock::getPriceChangeFromInitialPercent() const {
        if (initialPrice <= 0.0) return 0.0;
        return ((currentPrice - initialPrice) / initialPrice) * 100.0;
    }

    void Stock::updatePrice(double newPrice) {
        if (newPrice <= 0.0) {
            throw InvalidStockException("Stock price cannot be updated to a zero or negative value.");
        }
        previousPrice = currentPrice;
        currentPrice = newPrice;
        priceHistory.push_back(newPrice);
        
        // Prevent infinite growth of price history vector by capping it at 50 records
        if (priceHistory.size() > 50) {
            priceHistory.erase(priceHistory.begin());
        }
    }

    void Stock::addSharesTraded(int quantity) {
        if (quantity < 0) {
            throw InvalidShareQuantityException("Shares quantity traded cannot be negative.");
        }
        totalSharesTraded += quantity;
    }

    void Stock::setPreviousPrice(double price) {
        previousPrice = price;
    }

    void Stock::setInitialPrice(double price) {
        initialPrice = price;
    }

    void Stock::setTotalSharesTraded(int shares) {
        totalSharesTraded = shares;
    }

    void Stock::setPriceHistory(const std::vector<double>& history) {
        priceHistory = history;
    }

} // namespace StockMarket
