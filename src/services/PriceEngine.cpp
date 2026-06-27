#include "services/PriceEngine.h"
#include <vector>

namespace StockMarket {

    PriceEngine::PriceEngine(double minPercent, double maxPercent) 
        : distribution(minPercent, maxPercent) {
        std::random_device rd;
        generator.seed(rd());
    }

    void PriceEngine::refreshPrices(Market& market) {
        // Gather all tickers first to avoid modifying the container while iterating
        std::vector<std::string> tickers;
        for (const auto& pair : market.getStocks()) {
            tickers.push_back(pair.first);
        }

        for (const auto& ticker : tickers) {
            double currentPrice = market.getStock(ticker).getCurrentPrice();
            
            // Calculate random percentage movement
            double percentChange = distribution(generator);
            double multiplier = 1.0 + (percentChange / 100.0);
            double newPrice = currentPrice * multiplier;

            // Ensure stock price doesn't drop below a minimum threshold of $0.01
            if (newPrice < 0.01) {
                newPrice = 0.01;
            }

            market.updateStockPrice(ticker, newPrice);
        }
    }

} // namespace StockMarket
