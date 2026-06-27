#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "services/Market.h"
#include "models/User.h"
#include "models/Admin.h"
#include "models/Transaction.h"
#include <unordered_map>
#include <vector>
#include <string>

namespace StockMarket {

    /**
     * @brief Utility class to handle persistence of data to and from text files.
     * Demonstrates File Stream handling (fstream), CSV parsing, and String Tokenization.
     */
    class FileManager {
    public:
        // Market stocks persistence
        static void loadMarketData(Market& market, const std::string& filepath);
        static void saveMarketData(const Market& market, const std::string& filepath);

        // Account (User / Admin) registries persistence
        static void loadUserData(std::unordered_map<std::string, User>& users,
                                 std::unordered_map<std::string, Admin>& admins,
                                 const std::string& filepath);
        static void saveUserData(const std::unordered_map<std::string, User>& users,
                                 const std::unordered_map<std::string, Admin>& admins,
                                 const std::string& filepath);

        // Transaction history log persistence
        static std::vector<Transaction> loadTransactions(const std::string& filepath);
        static void saveTransaction(const Transaction& tx, const std::string& filepath); // Appends one trade
        static void saveAllTransactions(const std::vector<Transaction>& txs, const std::string& filepath); // Overwrites entire log
    };

} // namespace StockMarket

#endif // FILEMANAGER_H
