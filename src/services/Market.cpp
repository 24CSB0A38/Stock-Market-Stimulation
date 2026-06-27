#include "services/Market.h"
#include "models/User.h"
#include "utils/Exceptions.h"
#include "utils/Helper.h"
#include <queue>
#include <algorithm>
#include <iostream>
#include <iomanip>

namespace StockMarket {

    Market::Market() : isOpen(true) {}

    bool Market::isMarketOpen() const {
        return isOpen;
    }

    void Market::setMarketOpen(bool status) {
        isOpen = status;
    }

    void Market::addStock(const Stock& stock) {
        std::string tickerUpper = Utils::toUpperCase(stock.getTicker());
        if (stocks.find(tickerUpper) != stocks.end()) {
            throw InvalidStockException("Stock ticker '" + tickerUpper + "' already exists in the market.");
        }
        stocks[tickerUpper] = stock;
    }

    void Market::removeStock(const std::string& ticker, const std::unordered_map<std::string, User>& users) {
        std::string tickerUpper = Utils::toUpperCase(ticker);
        auto it = stocks.find(tickerUpper);
        if (it == stocks.end()) {
            throw StockNotFoundException("Ticker '" + tickerUpper + "' not found in market registry.");
        }

        // Business Rule: Admin cannot remove stock with active shareholders
        for (const auto& pair : users) {
            const auto& holdings = pair.second.getPortfolio().getHoldings();
            if (holdings.find(tickerUpper) != holdings.end()) {
                throw InvalidStockException("Cannot remove stock '" + tickerUpper + "': active shareholders exist.");
            }
        }

        stocks.erase(it);
    }

    void Market::updateStockPrice(const std::string& ticker, double newPrice) {
        std::string tickerUpper = Utils::toUpperCase(ticker);
        auto it = stocks.find(tickerUpper);
        if (it == stocks.end()) {
            throw StockNotFoundException("Ticker '" + tickerUpper + "' not found in market registry.");
        }
        it->second.updatePrice(newPrice);
    }

    bool Market::hasStock(const std::string& ticker) const {
        std::string tickerUpper = Utils::toUpperCase(ticker);
        return stocks.find(tickerUpper) != stocks.end();
    }

    const Stock& Market::getStock(const std::string& ticker) const {
        std::string tickerUpper = Utils::toUpperCase(ticker);
        auto it = stocks.find(tickerUpper);
        if (it == stocks.end()) {
            throw StockNotFoundException("Stock ticker '" + tickerUpper + "' not found.");
        }
        return it->second;
    }

    Stock& Market::getStockWritable(const std::string& ticker) {
        std::string tickerUpper = Utils::toUpperCase(ticker);
        auto it = stocks.find(tickerUpper);
        if (it == stocks.end()) {
            throw StockNotFoundException("Stock ticker '" + tickerUpper + "' not found.");
        }
        return it->second;
    }

    const std::unordered_map<std::string, Stock>& Market::getStocks() const {
        return stocks;
    }

    void Market::displayMarket() const {
        Utils::printHeader("Market Stock Directory");
        std::cout << "Market Status: " << (isOpen ? "OPEN" : "CLOSED") << "\n\n";
        
        std::cout << std::left 
                  << std::setw(8)  << "Ticker" 
                  << std::setw(22) << "Company Name" 
                  << std::setw(12) << "Price" 
                  << std::setw(12) << "Change" 
                  << std::setw(15) << "Shares Traded" << "\n";
        std::cout << std::string(69, '-') << "\n";

        // Sort stocks alphabetically by ticker for user presentation
        std::vector<Stock> sortedStocks;
        for (const auto& pair : stocks) {
            sortedStocks.push_back(pair.second);
        }
        std::sort(sortedStocks.begin(), sortedStocks.end(), [](const Stock& a, const Stock& b) {
            return a.getTicker() < b.getTicker();
        });

        for (const auto& stock : sortedStocks) {
            std::cout << std::left 
                      << std::setw(8)  << stock.getTicker() 
                      << std::setw(22) << stock.getCompanyName() 
                      << std::setw(12) << Utils::formatCurrency(stock.getCurrentPrice()) 
                      << std::setw(12) << Utils::formatPercentage(stock.getPriceChangePercent()) 
                      << std::setw(15) << stock.getTotalSharesTraded() << "\n";
        }
        std::cout << std::string(69, '-') << "\n";
    }

    // Comparators for Priority Queues
    struct GainerComp {
        bool operator()(const Stock& a, const Stock& b) {
            return a.getPriceChangePercent() < b.getPriceChangePercent(); // Max-heap: highest first
        }
    };

    struct LoserComp {
        bool operator()(const Stock& a, const Stock& b) {
            return a.getPriceChangePercent() > b.getPriceChangePercent(); // Min-heap: lowest first
        }
    };

    struct TradedComp {
        bool operator()(const Stock& a, const Stock& b) {
            return a.getTotalSharesTraded() < b.getTotalSharesTraded(); // Max-heap: most traded first
        }
    };

    struct VolumeValueComp {
        bool operator()(const Stock& a, const Stock& b) {
            double volA = a.getTotalSharesTraded() * a.getCurrentPrice();
            double volB = b.getTotalSharesTraded() * b.getCurrentPrice();
            return volA < volB; // Max-heap: highest trading volume first
        }
    };

    std::vector<Stock> Market::getTopGainers(size_t limit) const {
        std::priority_queue<Stock, std::vector<Stock>, GainerComp> pq;
        for (const auto& pair : stocks) {
            pq.push(pair.second);
        }
        
        std::vector<Stock> result;
        while (!pq.empty() && result.size() < limit) {
            result.push_back(pq.top());
            pq.pop();
        }
        return result;
    }

    std::vector<Stock> Market::getTopLosers(size_t limit) const {
        std::priority_queue<Stock, std::vector<Stock>, LoserComp> pq;
        for (const auto& pair : stocks) {
            pq.push(pair.second);
        }
        
        std::vector<Stock> result;
        while (!pq.empty() && result.size() < limit) {
            result.push_back(pq.top());
            pq.pop();
        }
        return result;
    }

    std::vector<Stock> Market::getMostTraded(size_t limit) const {
        std::priority_queue<Stock, std::vector<Stock>, TradedComp> pq;
        for (const auto& pair : stocks) {
            pq.push(pair.second);
        }
        
        std::vector<Stock> result;
        while (!pq.empty() && result.size() < limit) {
            result.push_back(pq.top());
            pq.pop();
        }
        return result;
    }

    std::vector<Stock> Market::getHighestVolumeValue(size_t limit) const {
        std::priority_queue<Stock, std::vector<Stock>, VolumeValueComp> pq;
        for (const auto& pair : stocks) {
            pq.push(pair.second);
        }
        
        std::vector<Stock> result;
        while (!pq.empty() && result.size() < limit) {
            result.push_back(pq.top());
            pq.pop();
        }
        return result;
    }

} // namespace StockMarket
