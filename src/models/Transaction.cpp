#include "models/Transaction.h"
#include <sstream>
#include <vector>

namespace StockMarket {

    Transaction::Transaction() 
        : transactionId(""), username(""), ticker(""), type(""), quantity(0), price(0.0), timestamp("") {}

    Transaction::Transaction(const std::string& transactionId, const std::string& username, const std::string& ticker, 
                             const std::string& type, int quantity, double price, 
                             const std::string& timestamp)
        : transactionId(transactionId), username(username), ticker(ticker), type(type), 
          quantity(quantity), price(price), timestamp(timestamp) {}

    std::string Transaction::getTransactionId() const { return transactionId; }
    std::string Transaction::getUsername() const { return username; }
    std::string Transaction::getTicker() const { return ticker; }
    std::string Transaction::getType() const { return type; }
    int Transaction::getQuantity() const { return quantity; }
    double Transaction::getPrice() const { return price; }
    std::string Transaction::getTimestamp() const { return timestamp; }

    std::string Transaction::serialize() const {
        std::ostringstream oss;
        oss << transactionId << "," << username << "," << ticker << "," << type << "," 
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
        
        if (tokens.size() < 7) {
            return Transaction();
        }
        
        std::string txId = tokens[0];
        std::string uname = tokens[1];
        std::string tickerStr = tokens[2];
        std::string typeStr = tokens[3];
        int qty = std::stoi(tokens[4]);
        double priceVal = std::stod(tokens[5]);
        std::string timeStr = tokens[6];
        
        return Transaction(txId, uname, tickerStr, typeStr, qty, priceVal, timeStr);
    }

} // namespace StockMarket
