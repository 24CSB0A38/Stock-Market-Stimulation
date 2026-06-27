#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>

namespace StockMarket {

    class Transaction {
    private:
        std::string transactionId;
        std::string username;
        std::string ticker;
        std::string type; // "BUY" or "SELL"
        int quantity;
        double price;
        std::string timestamp;

    public:
        // Default constructor
        Transaction();

        // Parameterized constructor
        Transaction(const std::string& transactionId, const std::string& username, const std::string& ticker, 
                    const std::string& type, int quantity, double price, 
                    const std::string& timestamp);
        
        std::string getUsername() const;
        // Accessors (Const correct)
        std::string getTransactionId() const;
        std::string getTicker() const;
        std::string getType() const;
        int getQuantity() const;
        double getPrice() const;
        std::string getTimestamp() const;

        // Formats transaction data as a single-line string for file storage
        std::string serialize() const;

        // Creates a Transaction object from a comma-separated text line
        static Transaction deserialize(const std::string& line);
    };

} // namespace StockMarket

#endif // TRANSACTION_H
