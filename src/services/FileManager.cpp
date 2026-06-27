#include "services/FileManager.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace StockMarket {

    void FileManager::loadMarketData(Market& market, const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return; // File doesn't exist yet, start with default empty registry
        }

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;

            std::stringstream ss(line);
            std::string ticker, name, currentStr, prevStr, initStr, tradedStr, historyStr;

            std::getline(ss, ticker, ',');
            std::getline(ss, name, ',');
            std::getline(ss, currentStr, ',');
            std::getline(ss, prevStr, ',');
            std::getline(ss, initStr, ',');
            std::getline(ss, tradedStr, ',');
            std::getline(ss, historyStr, ',');

            try {
                Stock stock(ticker, name, std::stod(currentStr));
                stock.setPreviousPrice(std::stod(prevStr));
                stock.setInitialPrice(std::stod(initStr));
                stock.setTotalSharesTraded(std::stoi(tradedStr));

                // Parse history string (separated by semicolons)
                std::stringstream histSS(historyStr);
                std::string priceVal;
                std::vector<double> history;
                while (std::getline(histSS, priceVal, ';')) {
                    if (!priceVal.empty()) {
                        history.push_back(std::stod(priceVal));
                    }
                }
                stock.setPriceHistory(history);
                market.addStock(stock);
            } catch (...) {
                // If any line fails parser validation, skip it to preserve database safety
                continue;
            }
        }
    }

    void FileManager::saveMarketData(const Market& market, const std::string& filepath) {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "Error: Unable to open file for saving stock data: " << filepath << "\n";
            return;
        }

        for (const auto& pair : market.getStocks()) {
            const Stock& stock = pair.second;
            file << stock.getTicker() << ","
                 << stock.getCompanyName() << ","
                 << stock.getCurrentPrice() << ","
                 << stock.getPreviousPrice() << ","
                 << stock.getInitialPrice() << ","
                 << stock.getTotalSharesTraded() << ",";

            const auto& history = stock.getPriceHistory();
            for (size_t i = 0; i < history.size(); ++i) {
                file << history[i];
                if (i < history.size() - 1) {
                    file << ";";
                }
            }
            file << "\n";
        }
    }

    void FileManager::loadUserData(std::unordered_map<std::string, User>& users,
                                     std::unordered_map<std::string, Admin>& admins,
                                     const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return; // File doesn't exist yet, start with empty account lists
        }

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;

            std::stringstream ss(line);
            std::string username, password, role, balanceStr, watchlistStr, holdingsStr;

            std::getline(ss, username, ',');
            std::getline(ss, password, ',');
            std::getline(ss, role, ',');
            std::getline(ss, balanceStr, ',');
            std::getline(ss, watchlistStr, ',');
            std::getline(ss, holdingsStr, ',');

            try {
                if (role == "ADMIN") {
                    admins[username] = Admin(username, password);
                } else if (role == "USER") {
                    User user(username, password, std::stod(balanceStr));

                    // Load Watchlist (separated by semicolons)
                    std::stringstream watchSS(watchlistStr);
                    std::string watchTicker;
                    while (std::getline(watchSS, watchTicker, ';')) {
                        if (!watchTicker.empty()) {
                            user.addToWatchlist(watchTicker);
                        }
                    }

                    // Load Portfolio Holdings (separated by semicolons e.g., AAPL:10:150.5;MSFT:5:280)
                    std::stringstream holdSS(holdingsStr);
                    std::string holdingItem;
                    while (std::getline(holdSS, holdingItem, ';')) {
                        if (holdingItem.empty()) continue;
                        
                        std::stringstream itemSS(holdingItem);
                        std::string hTicker, hSharesStr, hAvgPriceStr;
                        std::getline(itemSS, hTicker, ':');
                        std::getline(itemSS, hSharesStr, ':');
                        std::getline(itemSS, hAvgPriceStr, ':');

                        if (!hTicker.empty() && !hSharesStr.empty() && !hAvgPriceStr.empty()) {
                            user.getPortfolio().buy(hTicker, std::stoi(hSharesStr), std::stod(hAvgPriceStr));
                        }
                    }

                    users[username] = user;
                }
            } catch (...) {
                // Skip malformed records to guarantee integrity
                continue;
            }
        }
    }

    void FileManager::saveUserData(const std::unordered_map<std::string, User>& users,
                                     const std::unordered_map<std::string, Admin>& admins,
                                     const std::string& filepath) {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "Error: Unable to open file for saving user accounts: " << filepath << "\n";
            return;
        }

        // Save Admins
        for (const auto& pair : admins) {
            file << pair.first << ","
                 << pair.second.getPassword() << ","
                 << "ADMIN,0.0,,\n";
        }

        // Save Users
        for (const auto& pair : users) {
            const User& user = pair.second;
            file << user.getUsername() << ","
                 << user.getPassword() << ","
                 << "USER,"
                 << user.getBalance() << ",";

            // Watchlist serialization
            const auto& watchlist = user.getWatchlist();
            size_t count = 0;
            for (const auto& ticker : watchlist) {
                file << ticker;
                if (++count < watchlist.size()) {
                    file << ";";
                }
            }
            file << ",";

            // Portfolio holdings serialization
            const auto& holdings = user.getPortfolio().getHoldings();
            count = 0;
            for (const auto& hPair : holdings) {
                file << hPair.first << ":"
                     << hPair.second.shares << ":"
                     << hPair.second.averageBuyPrice;
                if (++count < holdings.size()) {
                    file << ";";
                }
            }
            file << "\n";
        }
    }

    std::vector<Transaction> FileManager::loadTransactions(const std::string& filepath) {
        std::vector<Transaction> transactions;
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return transactions; // Return empty vector if no ledger is found
        }

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            Transaction tx = Transaction::deserialize(line);
            if (!tx.getTransactionId().empty()) {
                transactions.push_back(tx);
            }
        }
        return transactions;
    }

    void FileManager::saveTransaction(const Transaction& tx, const std::string& filepath) {
        std::ofstream file(filepath, std::ios::app); // Open in append mode
        if (!file.is_open()) {
            std::cerr << "Error: Unable to open file for appending transaction: " << filepath << "\n";
            return;
        }
        file << tx.serialize() << "\n";
    }

    void FileManager::saveAllTransactions(const std::vector<Transaction>& txs, const std::string& filepath) {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "Error: Unable to open file for saving transactions: " << filepath << "\n";
            return;
        }
        for (const auto& tx : txs) {
            file << tx.serialize() << "\n";
        }
    }

} // namespace StockMarket
