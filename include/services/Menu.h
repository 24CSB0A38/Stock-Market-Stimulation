#ifndef MENU_H
#define MENU_H

#include "services/Market.h"
#include "services/Authentication.h"
#include "services/PriceEngine.h"
#include "models/Transaction.h"
#include <vector>
#include <string>

namespace StockMarket {

    /**
     * @brief CLI controller that renders menus and routes user actions.
     * Demonstrates Single Responsibility Principle and Dependency Injection.
     * 
     * Menu holds references to the live services. It does NOT own them.
     * All objects are created and destroyed in main.cpp.
     */
    class Menu {
    private:
        Market& market;
        Authentication& auth;
        PriceEngine& priceEngine;
        std::vector<Transaction>& transactionLog;

        const std::string DATA_USERS_FILE        = "data/users.txt";
        const std::string DATA_STOCKS_FILE       = "data/stocks.txt";
        const std::string DATA_TRANSACTIONS_FILE = "data/transactions.txt";

        // Helper: generates a unique transaction ID
        std::string generateTransactionId() const;

        // --- Landing & Auth Screens ---
        void showWelcomeScreen() const;
        void handleRegister();
        void handleLogin();

        // --- Main Role Dispatchers ---
        void showUserMenu();
        void showAdminMenu();

        // --- User Sub-Menus ---
        void handleViewProfile() const;
        void handleMarketDisplay() const;
        void handleSearchStock() const;
        void handleBuyStock();
        void handleSellStock();
        void handleViewPortfolio() const;
        void handleViewTransactionHistory() const;
        void handleWatchlist();
        void handleAnalytics() const;
        void handleRefreshPrices();

        // --- Admin Sub-Menus ---
        void handleAdminProfile() const;
        void handleAddStock();
        void handleRemoveStock();
        void handleUpdateStockPrice();
        void handleAdminAnalytics() const;
        void handleMarketStatusToggle();

        // --- Shared Utilities ---
        void displayAnalyticsResults() const;
        void displayUserAnalytics() const;

    public:
        Menu(Market& market, Authentication& auth, PriceEngine& priceEngine,
             std::vector<Transaction>& transactionLog);

        // Primary entry point — runs the full application lifecycle
        void run();
    };

} // namespace StockMarket

#endif // MENU_H
