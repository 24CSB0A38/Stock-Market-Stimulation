#ifndef PORTFOLIO_H
#define PORTFOLIO_H

#include <map>
#include <string>

namespace StockMarket {

    // Forward declaration of Market to break circular dependency
    class Market;

    // Struct representing an individual stock holding in a user's portfolio
    struct Holding {
        std::string ticker;
        int shares;
        double averageBuyPrice;
    };

    /**
     * @brief Manages a collection of stock holdings.
     * Demonstrates Composition, Encapsulation, and dynamic calculation logic.
     */
    class Portfolio {
    private:
        // map holds holdings sorted alphabetically by ticker naturally
        std::map<std::string, Holding> holdings;

    public:
        Portfolio();

        // Getters
        const std::map<std::string, Holding>& getHoldings() const;

        // Trade modifiers
        void buy(const std::string& ticker, int quantity, double price);
        void sell(const std::string& ticker, int quantity);

        // Dynamic Portfolio Analytics (Calculated on the fly, never stored)
        double getTotalInvestment() const;
        double getCurrentValue(const Market& market) const;
        double getNetProfitLoss(const Market& market) const; 
        double getROI(const Market& market) const;
    };

} // namespace StockMarket

#endif // PORTFOLIO_H
