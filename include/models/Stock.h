#ifndef STOCK_H
#define STOCK_H

#include <string>
#include <vector>

namespace StockMarket {

    class Stock {
    private:
        std::string ticker;
        std::string companyName;
        double currentPrice;
        double previousPrice;
        double initialPrice;
        int totalSharesTraded;
        std::vector<double> priceHistory;

    public:
        // Default constructor (required for std::unordered_map insertion)
        Stock();

        // Parameterized constructor
        Stock(const std::string& ticker, const std::string& companyName, 
              double price);

        // Accessors (Const correct)
        std::string getTicker() const;
        std::string getCompanyName() const;
        double getCurrentPrice() const;
        double getPreviousPrice() const;
        double getInitialPrice() const;
        int getTotalSharesTraded() const;
        const std::vector<double>& getPriceHistory() const;
        
        // Dynamically calculates percentage change from previous price
        double getPriceChangePercent() const;

        // Dynamically calculates percentage change from initial price
        double getPriceChangeFromInitialPercent() const;

        // Mutators
        void updatePrice(double newPrice);
        void addSharesTraded(int quantity);
        void setPreviousPrice(double price);
        void setInitialPrice(double price);
        void setTotalSharesTraded(int shares);
        void setPriceHistory(const std::vector<double>& history);
    };

} // namespace StockMarket

#endif // STOCK_H
