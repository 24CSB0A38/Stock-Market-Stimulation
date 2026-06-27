/*
 * ============================================================================
 *  Models.h — All Data Model Declarations
 * ============================================================================
 *  Contains: Stock, Transaction, Holding, Portfolio, Account, User, Admin
 * ============================================================================
 */

#ifndef MODELS_H
#define MODELS_H

#include "Utils.h"
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>

using namespace std;

// Forward declaration to break circular dependency (Portfolio needs Market)
class Market;

// =============================================================================
// STOCK
// =============================================================================

class Stock {
private:
    string ticker;
    string companyName;
    double currentPrice;
    double previousPrice;
    double initialPrice;
    int totalSharesTraded;
    vector<double> priceHistory;

public:
    Stock();
    Stock(const string& ticker, const string& companyName, double price);

    // Accessors
    string getTicker() const;
    string getCompanyName() const;
    double getCurrentPrice() const;
    double getPreviousPrice() const;
    double getInitialPrice() const;
    int getTotalSharesTraded() const;
    const vector<double>& getPriceHistory() const;
    double getPriceChangePercent() const;
    double getPriceChangeFromInitialPercent() const;

    // Mutators
    void updatePrice(double newPrice);
    void addSharesTraded(int quantity);
    void setPreviousPrice(double price);
    void setInitialPrice(double price);
    void setTotalSharesTraded(int shares);
    void setPriceHistory(const vector<double>& history);
};

// =============================================================================
// TRANSACTION
// =============================================================================

class Transaction {
private:
    string transactionId;
    string username;
    string ticker;
    string type; // "BUY" or "SELL"
    int quantity;
    double price;
    string timestamp;

public:
    Transaction();
    Transaction(const string& transactionId, const string& username, const string& ticker,
                const string& type, int quantity, double price, const string& timestamp);

    string getTransactionId() const;
    string getUsername() const;
    string getTicker() const;
    string getType() const;
    int getQuantity() const;
    double getPrice() const;
    string getTimestamp() const;

    string serialize() const;
    static Transaction deserialize(const string& line);
};

// =============================================================================
// HOLDING & PORTFOLIO
// =============================================================================

struct Holding {
    string ticker;
    int shares;
    double averageBuyPrice;
};

class Portfolio {
private:
    map<string, Holding> holdings;

public:
    Portfolio();

    const map<string, Holding>& getHoldings() const;

    void buy(const string& ticker, int quantity, double price);
    void sell(const string& ticker, int quantity);

    // Dynamic analytics (calculated on the fly, never stored)
    double getTotalInvestment() const;
    double getCurrentValue(const Market& market) const;
    double getNetProfitLoss(const Market& market) const;
    double getROI(const Market& market) const;
};

// =============================================================================
// ACCOUNT (Abstract Base Class)
// =============================================================================

class Account {
protected:
    string username;
    string password;
    string role; // "USER" or "ADMIN"

public:
    Account(const string& username, const string& password, const string& role)
        : username(username), password(password), role(role) {}

    virtual ~Account() = default;

    string getUsername() const { return username; }
    string getPassword() const { return password; }
    string getRole() const { return role; }

    // Pure virtual — forces derived classes to implement their own profile display
    virtual void displayProfile() const = 0;
};

// =============================================================================
// USER (Inherits Account)
// =============================================================================

class User : public Account {
private:
    double balance;
    Portfolio portfolio;
    unordered_set<string> watchlist;

public:
    User();
    User(const string& username, const string& password, double initialBalance = 10000.0);

    void displayProfile() const override;

    double getBalance() const;
    void creditBalance(double amount);
    void debitBalance(double amount);

    Portfolio& getPortfolio();
    const Portfolio& getPortfolio() const;

    const unordered_set<string>& getWatchlist() const;
    void addToWatchlist(const string& ticker);
    void removeFromWatchlist(const string& ticker);
};

// =============================================================================
// ADMIN (Inherits Account)
// =============================================================================

class Admin : public Account {
public:
    Admin();
    Admin(const string& username, const string& password);

    void displayProfile() const override;
};

#endif // MODELS_H
