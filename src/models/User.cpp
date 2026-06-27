#include "models/User.h"
#include "utils/Exceptions.h"
#include "utils/Helper.h"
#include <iostream>

namespace StockMarket {

    User::User() 
        : Account("", "", "USER"), balance(0.0) {}

    User::User(const std::string& username, const std::string& password, double initialBalance)
        : Account(username, password, "USER"), balance(initialBalance) {
        if (initialBalance < 0.0) {
            throw InsufficientBalanceException("Initial balance cannot be negative.");
        }
    }

    void User::displayProfile() const {
        std::cout << "--- User Profile ---\n";
        std::cout << "Username:       " << username << "\n";
        std::cout << "Role:           " << role << "\n";
        std::cout << "Wallet Balance: " << Utils::formatCurrency(balance) << "\n";
        std::cout << "Holdings Count: " << portfolio.getHoldings().size() << "\n";
        std::cout << "Watchlist Size: " << watchlist.size() << " stocks\n";
    }

    double User::getBalance() const {
        return balance;
    }

    void User::creditBalance(double amount) {
        if (amount < 0.0) {
            throw InsufficientBalanceException("Cannot credit a negative amount.");
        }
        balance += amount;
    }

    void User::debitBalance(double amount) {
        if (amount < 0.0) {
            throw InsufficientBalanceException("Cannot debit a negative amount.");
        }
        if (balance < amount) {
            throw InsufficientBalanceException("Insufficient wallet balance.");
        }
        balance -= amount;
    }

    Portfolio& User::getPortfolio() {
        return portfolio;
    }

    const Portfolio& User::getPortfolio() const {
        return portfolio;
    }

    const std::unordered_set<std::string>& User::getWatchlist() const {
        return watchlist;
    }

    void User::addToWatchlist(const std::string& ticker) {
        watchlist.insert(ticker);
    }

    void User::removeFromWatchlist(const std::string& ticker) {
        watchlist.erase(ticker);
    }

} // namespace StockMarket
