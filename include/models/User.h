#ifndef USER_H
#define USER_H

#include "models/Account.h"
#include "models/Portfolio.h"
#include <unordered_set>
#include <string>

namespace StockMarket {

    /**
     * @brief Class representing a regular user client (trader).
     * Demonstrates Inheritance, Composition (owns Portfolio and Watchlist), and Abstraction.
     */
    class User : public Account {
    private:
        double balance;
        Portfolio portfolio;
        std::unordered_set<std::string> watchlist;

    public:
        // Default constructor
        User();

        // Parameterized constructor
        User(const std::string& username, const std::string& password, double initialBalance = 10000.0);

        // Polymorphic implementation of displayProfile
        void displayProfile() const override;

        // Accessors & Mutators
        double getBalance() const;
        void creditBalance(double amount);
        void debitBalance(double amount);

        Portfolio& getPortfolio();
        const Portfolio& getPortfolio() const;

        const std::unordered_set<std::string>& getWatchlist() const;
        void addToWatchlist(const std::string& ticker);
        void removeFromWatchlist(const std::string& ticker);
    };

} // namespace StockMarket

#endif // USER_H
