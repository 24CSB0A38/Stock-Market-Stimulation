#ifndef MARKET_H
#define MARKET_H

#include "models/Stock.h"
#include <unordered_map>
#include <string>
#include <vector>

namespace StockMarket {

    // Forward declaration of User for rule checks
    class User;

    /**
     * @brief Manages the list of active stocks, coordinates transactions, and aggregates analytics.
     * Demonstrates Encapsulation, STL Algorithm utilization, and Priority Queues.
     */
    class Market {
    private:
        std::unordered_map<std::string, Stock> stocks;
        bool isOpen;

    public:
        Market();

        // Status accessors
        bool isMarketOpen() const;
        void setMarketOpen(bool status);

        // Core stock operations
        void addStock(const Stock& stock);
        void removeStock(const std::string& ticker, const std::unordered_map<std::string, User>& users);
        void updateStockPrice(const std::string& ticker, double newPrice);

        // Getters
        bool hasStock(const std::string& ticker) const;
        const Stock& getStock(const std::string& ticker) const;
        Stock& getStockWritable(const std::string& ticker); // non-const helper for transaction volume updates
        const std::unordered_map<std::string, Stock>& getStocks() const;

        // Display utility
        void displayMarket() const;

        // Advanced Analytics (priority_queue & algorithms)
        std::vector<Stock> getTopGainers(size_t limit = 5) const;
        std::vector<Stock> getTopLosers(size_t limit = 5) const;
        std::vector<Stock> getMostTraded(size_t limit = 5) const;
        std::vector<Stock> getHighestVolumeValue(size_t limit = 5) const;
    };

} // namespace StockMarket

#endif // MARKET_H
