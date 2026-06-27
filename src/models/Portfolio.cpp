#include "models/Portfolio.h"
#include "services/Market.h"
#include "utils/Exceptions.h"

namespace StockMarket {

    Portfolio::Portfolio() {}

    const std::map<std::string, Holding>& Portfolio::getHoldings() const {
        return holdings;
    }

    void Portfolio::buy(const std::string& ticker, int quantity, double price) {
        if (quantity <= 0) {
            throw InvalidShareQuantityException("Buy quantity must be greater than zero.");
        }
        if (price <= 0.0) {
            throw InvalidStockException("Stock purchase price must be greater than zero.");
        }

        auto it = holdings.find(ticker);
        if (it == holdings.end()) {
            // New position
            holdings[ticker] = {ticker, quantity, price};
        } else {
            // Add to existing position and recalculate weighted average buy price
            int currentShares = it->second.shares;
            double currentAvg = it->second.averageBuyPrice;
            
            double totalCost = (currentShares * currentAvg) + (quantity * price);
            int totalShares = currentShares + quantity;
            
            it->second.shares = totalShares;
            it->second.averageBuyPrice = totalCost / totalShares;
        }
    }

    void Portfolio::sell(const std::string& ticker, int quantity) {
        if (quantity <= 0) {
            throw InvalidShareQuantityException("Sell quantity must be greater than zero.");
        }

        auto it = holdings.find(ticker);
        if (it == holdings.end() || it->second.shares < quantity) {
            throw InvalidShareQuantityException("Insufficient shares owned. Cannot complete sale.");
        }

        it->second.shares -= quantity;
        
        // If all shares sold, liquidate the holding entry from the portfolio map
        if (it->second.shares == 0) {
            holdings.erase(it);
        }
    }

    double Portfolio::getTotalInvestment() const {
        double total = 0.0;
        for (const auto& pair : holdings) {
            total += pair.second.shares * pair.second.averageBuyPrice;
        }
        return total;
    }

    double Portfolio::getCurrentValue(const Market& market) const {
        double total = 0.0;
        for (const auto& pair : holdings) {
            // Retrieve current stock price from market dynamically
            double currentPrice = market.getStock(pair.first).getCurrentPrice();
            total += pair.second.shares * currentPrice;
        }
        return total;
    }

    double Portfolio::getNetProfitLoss(const Market& market) const {
        return getCurrentValue(market) - getTotalInvestment();
    }

    double Portfolio::getROI(const Market& market) const {
        double investment = getTotalInvestment();
        if (investment <= 0.0) return 0.0;
        return (getNetProfitLoss(market) / investment) * 100.0;
    }

} // namespace StockMarket
