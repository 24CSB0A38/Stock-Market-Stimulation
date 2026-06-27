#ifndef PRICEENGINE_H
#define PRICEENGINE_H

#include "services/Market.h"
#include <random>

namespace StockMarket {

    /**
     * @brief Simulates stock market price fluctuations using C++11 standard random number generation.
     * Demonstrates modern random distributions and mathematical logic.
     */
    class PriceEngine {
    private:
        // C++11 Mersenne Twister engine for high-quality random seeds
        std::mt19937 generator;
        
        // Distribution of percentage changes (e.g. from -5% to +5%)
        std::uniform_real_distribution<double> distribution;

    public:
        // Parameterized constructor allowing bounds configuration
        PriceEngine(double minPercent = -5.0, double maxPercent = 5.0);

        // Updates all stock prices in the market using random walk
        void refreshPrices(Market& market);
    };

} // namespace StockMarket

#endif // PRICEENGINE_H
