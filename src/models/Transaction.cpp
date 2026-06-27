#include "models/Transaction.h"
#include <sstream>
#include <vector>

namespace StockMarket {

    Transaction::Transaction() 
        : transactionId(""), ticker(""), type(""), quantity(0), price(0.0), timestamp("") {}

    Transaction::Transaction(const std::string& transactionId, const std::string& ticker, 
                             const std::string& type, int quantity, double price, 
                             const std::string& timestamp)
        : transactionId(transactionId), ticker(ticker), type(type), 
          quantity(quantity), price(price), timestamp(timestamp) {}

    std::string Transaction::getTransactionId() const { return transactionId; }
    std::string Transaction::getTicker() const { return ticker; }
    std::string Transaction::getType() const { return type; }
    int Transaction::getQuantity() const { return quantity; }
    double Transaction::getPrice() const { return price; }
    std::string Transaction::getTimestamp() const { return timestamp; }

    std::string Transaction::serialize() const {
        std::ostringstream oss;
        oss << transactionId << "," << ticker << "," << type << "," 
            << quantity << "," << price << "," << timestamp;
        return oss.str();
    }

    Transaction Transaction::deserialize(const std::string& line) {
        std::stringstream ss(line);
        std::string item;
        std::vector<std::string> tokens;
        
        while (std::getline(ss, item, ',')) {
            tokens.push_back(item);
        }
        
        if (tokens.size() < 6) {
            return Transaction();
        }
        
        std::string txId = tokens[0];
        std::string tickerStr = tokens[1];
        std::string typeStr = tokens[2];
        int qty = std::stoi(tokens[3]);
        double priceVal = std::stod(tokens[4]);
        std::string timeStr = tokens[5];
        
        return Transaction(txId, tickerStr, typeStr, qty, priceVal, timeStr);
    }

} // namespace StockMarket
