/*
 * ============================================================================
 *  Services.h — All Service Class Declarations
 * ============================================================================
 *  Contains: Market, PriceEngine, FileManager, Authentication, Menu
 * ============================================================================
 */

#ifndef SERVICES_H
#define SERVICES_H

#include "Models.h"
#include <unordered_map>
#include <vector>
#include <random>

using namespace std;

// =============================================================================
// MARKET
// =============================================================================

class Market {
private:
    unordered_map<string, Stock> stocks;
    bool isOpen;

public:
    Market();

    bool isMarketOpen() const;
    void setMarketOpen(bool status);

    void addStock(const Stock& stock);
    void removeStock(const string& ticker, const unordered_map<string, User>& users);
    void updateStockPrice(const string& ticker, double newPrice);

    bool hasStock(const string& ticker) const;
    const Stock& getStock(const string& ticker) const;
    Stock& getStockWritable(const string& ticker);
    const unordered_map<string, Stock>& getStocks() const;

    void displayMarket() const;

    // Analytics (uses priority_queue with custom comparators)
    vector<Stock> getTopGainers(size_t limit = 5) const;
    vector<Stock> getTopLosers(size_t limit = 5) const;
    vector<Stock> getMostTraded(size_t limit = 5) const;
    vector<Stock> getHighestVolumeValue(size_t limit = 5) const;
};

// =============================================================================
// PRICE ENGINE
// =============================================================================

class PriceEngine {
private:
    mt19937 generator;
    uniform_real_distribution<double> distribution;

public:
    PriceEngine(double minPercent = -5.0, double maxPercent = 5.0);
    void refreshPrices(Market& market);
};

// =============================================================================
// FILE MANAGER
// =============================================================================

class FileManager {
public:
    static void loadMarketData(Market& market, const string& filepath);
    static void saveMarketData(const Market& market, const string& filepath);

    static void loadUserData(unordered_map<string, User>& users,
                             unordered_map<string, Admin>& admins,
                             const string& filepath);
    static void saveUserData(const unordered_map<string, User>& users,
                             const unordered_map<string, Admin>& admins,
                             const string& filepath);

    static vector<Transaction> loadTransactions(const string& filepath);
    static void saveTransaction(const Transaction& tx, const string& filepath);
    static void saveAllTransactions(const vector<Transaction>& txs, const string& filepath);
};

// =============================================================================
// AUTHENTICATION
// =============================================================================

class Authentication {
private:
    unordered_map<string, User> users;
    unordered_map<string, Admin> admins;
    Account* currentUser; // Non-owning pointer into users or admins map

public:
    Authentication();

    void registerUser(const string& username, const string& password, double initialBalance = 10000.0);
    void registerAdmin(const string& username, const string& password);
    void login(const string& username, const string& password);
    void logout();

    Account* getCurrentUser() const;
    bool isLoggedIn() const;

    User& getCurrentUserAsUser();
    const User& getCurrentUserAsUser() const;

    unordered_map<string, User>& getUsers();
    const unordered_map<string, User>& getUsers() const;
    unordered_map<string, Admin>& getAdmins();
    const unordered_map<string, Admin>& getAdmins() const;

    void loadUsersFromFile(const string& filepath);
    void saveUsersToFile(const string& filepath) const;

    pair<string, double> getHighestPortfolioValueUser(const Market& market) const;
    pair<string, double> getHighestROIUser(const Market& market) const;
};

// =============================================================================
// MENU (CLI Controller)
// =============================================================================

class Menu {
private:
    Market& market;
    Authentication& auth;
    PriceEngine& priceEngine;
    vector<Transaction>& transactionLog;

    const string DATA_USERS_FILE        = "data/users.txt";
    const string DATA_STOCKS_FILE       = "data/stocks.txt";
    const string DATA_TRANSACTIONS_FILE = "data/transactions.txt";

    string generateTransactionId() const;

    void showWelcomeScreen() const;
    void handleRegister();
    void handleLogin();

    void showUserMenu();
    void showAdminMenu();

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

    void handleAdminProfile() const;
    void handleAddStock();
    void handleRemoveStock();
    void handleUpdateStockPrice();
    void handleAdminAnalytics() const;
    void handleMarketStatusToggle();

    void displayAnalyticsResults() const;
    void displayUserAnalytics() const;

public:
    Menu(Market& market, Authentication& auth, PriceEngine& priceEngine,
         vector<Transaction>& transactionLog);
    void run();
};

#endif // SERVICES_H
