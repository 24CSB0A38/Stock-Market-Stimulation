/*
 * ============================================================================
 *  Models.cpp — All Data Model Implementations
 * ============================================================================
 */

#include "Models.h"
#include "Services.h"

using namespace std;

// =============================================================================
// STOCK IMPLEMENTATION
// =============================================================================

Stock::Stock()
    : ticker(""), companyName(""), currentPrice(0.0), previousPrice(0.0),
      initialPrice(0.0), totalSharesTraded(0) {}

Stock::Stock(const string& ticker, const string& companyName, double price)
    : ticker(ticker), companyName(companyName), currentPrice(price),
      previousPrice(price), initialPrice(price), totalSharesTraded(0) {
    if (price <= 0.0) {
        throw InvalidStockException("Initial stock price must be greater than zero.");
    }
    priceHistory.push_back(price);
}

string Stock::getTicker() const { return ticker; }
string Stock::getCompanyName() const { return companyName; }
double Stock::getCurrentPrice() const { return currentPrice; }
double Stock::getPreviousPrice() const { return previousPrice; }
double Stock::getInitialPrice() const { return initialPrice; }
int Stock::getTotalSharesTraded() const { return totalSharesTraded; }
const vector<double>& Stock::getPriceHistory() const { return priceHistory; }

double Stock::getPriceChangePercent() const {
    if (previousPrice <= 0.0) return 0.0;
    return ((currentPrice - previousPrice) / previousPrice) * 100.0;
}

double Stock::getPriceChangeFromInitialPercent() const {
    if (initialPrice <= 0.0) return 0.0;
    return ((currentPrice - initialPrice) / initialPrice) * 100.0;
}

void Stock::updatePrice(double newPrice) {
    if (newPrice <= 0.0) {
        throw InvalidStockException("Stock price cannot be updated to a zero or negative value.");
    }
    previousPrice = currentPrice;
    currentPrice = newPrice;
    priceHistory.push_back(newPrice);

    // Prevent infinite growth of price history by capping at 50 records
    if (priceHistory.size() > 50) {
        priceHistory.erase(priceHistory.begin());
    }
}

void Stock::addSharesTraded(int quantity) {
    if (quantity < 0) {
        throw InvalidShareQuantityException("Shares quantity traded cannot be negative.");
    }
    totalSharesTraded += quantity;
}

void Stock::setPreviousPrice(double price) { previousPrice = price; }
void Stock::setInitialPrice(double price) { initialPrice = price; }
void Stock::setTotalSharesTraded(int shares) { totalSharesTraded = shares; }
void Stock::setPriceHistory(const vector<double>& history) { priceHistory = history; }

// =============================================================================
// TRANSACTION IMPLEMENTATION
// =============================================================================

Transaction::Transaction()
    : transactionId(""), username(""), ticker(""), type(""), quantity(0), price(0.0), timestamp("") {}

Transaction::Transaction(const string& transactionId, const string& username, const string& ticker,
                         const string& type, int quantity, double price, const string& timestamp)
    : transactionId(transactionId), username(username), ticker(ticker), type(type),
      quantity(quantity), price(price), timestamp(timestamp) {}

string Transaction::getTransactionId() const { return transactionId; }
string Transaction::getUsername() const { return username; }
string Transaction::getTicker() const { return ticker; }
string Transaction::getType() const { return type; }
int Transaction::getQuantity() const { return quantity; }
double Transaction::getPrice() const { return price; }
string Transaction::getTimestamp() const { return timestamp; }

string Transaction::serialize() const {
    ostringstream oss;
    oss << transactionId << "," << username << "," << ticker << "," << type << ","
        << quantity << "," << price << "," << timestamp;
    return oss.str();
}

Transaction Transaction::deserialize(const string& line) {
    stringstream ss(line);
    string item;
    vector<string> tokens;

    while (getline(ss, item, ',')) {
        tokens.push_back(item);
    }

    if (tokens.size() < 7) {
        return Transaction();
    }

    return Transaction(tokens[0], tokens[1], tokens[2], tokens[3],
                       stoi(tokens[4]), stod(tokens[5]), tokens[6]);
}

// =============================================================================
// PORTFOLIO IMPLEMENTATION
// =============================================================================

Portfolio::Portfolio() {}

const map<string, Holding>& Portfolio::getHoldings() const { return holdings; }

void Portfolio::buy(const string& ticker, int quantity, double price) {
    if (quantity <= 0) {
        throw InvalidShareQuantityException("Buy quantity must be greater than zero.");
    }
    if (price <= 0.0) {
        throw InvalidStockException("Stock purchase price must be greater than zero.");
    }

    auto it = holdings.find(ticker);
    if (it == holdings.end()) {
        holdings[ticker] = {ticker, quantity, price};
    } else {
        // Recalculate weighted average buy price
        int currentShares = it->second.shares;
        double currentAvg = it->second.averageBuyPrice;

        double totalCost = (currentShares * currentAvg) + (quantity * price);
        int totalShares = currentShares + quantity;

        it->second.shares = totalShares;
        it->second.averageBuyPrice = totalCost / totalShares;
    }
}

void Portfolio::sell(const string& ticker, int quantity) {
    if (quantity <= 0) {
        throw InvalidShareQuantityException("Sell quantity must be greater than zero.");
    }

    auto it = holdings.find(ticker);
    if (it == holdings.end() || it->second.shares < quantity) {
        throw InvalidShareQuantityException("Insufficient shares owned. Cannot complete sale.");
    }

    it->second.shares -= quantity;

    // If all shares sold, remove the holding entry
    if (it->second.shares == 0) {
        holdings.erase(it);
    }
}

double Portfolio::getTotalInvestment() const {
    double total = 0.0;
    for (const auto& pair : holdings) {
        total += pair.second.shares * pair.second.averageBuyPrice;
    }
    return total;
}

double Portfolio::getCurrentValue(const Market& market) const {
    double total = 0.0;
    for (const auto& pair : holdings) {
        double currentPrice = market.getStock(pair.first).getCurrentPrice();
        total += pair.second.shares * currentPrice;
    }
    return total;
}

double Portfolio::getNetProfitLoss(const Market& market) const {
    return getCurrentValue(market) - getTotalInvestment();
}

double Portfolio::getROI(const Market& market) const {
    double investment = getTotalInvestment();
    if (investment <= 0.0) return 0.0;
    return (getNetProfitLoss(market) / investment) * 100.0;
}

// =============================================================================
// USER IMPLEMENTATION
// =============================================================================

User::User() : Account("", "", "USER"), balance(0.0) {}

User::User(const string& username, const string& password, double initialBalance)
    : Account(username, password, "USER"), balance(initialBalance) {
    if (initialBalance < 0.0) {
        throw InsufficientBalanceException("Initial balance cannot be negative.");
    }
}

void User::displayProfile() const {
    cout << "--- User Profile ---\n";
    cout << "Username:       " << username << "\n";
    cout << "Role:           " << role << "\n";
    cout << "Wallet Balance: " << Utils::formatCurrency(balance) << "\n";
    cout << "Holdings Count: " << portfolio.getHoldings().size() << "\n";
    cout << "Watchlist Size: " << watchlist.size() << " stocks\n";
}

double User::getBalance() const { return balance; }

void User::creditBalance(double amount) {
    if (amount < 0.0) throw InsufficientBalanceException("Cannot credit a negative amount.");
    balance += amount;
}

void User::debitBalance(double amount) {
    if (amount < 0.0) throw InsufficientBalanceException("Cannot debit a negative amount.");
    if (balance < amount) throw InsufficientBalanceException("Insufficient wallet balance.");
    balance -= amount;
}

Portfolio& User::getPortfolio() { return portfolio; }
const Portfolio& User::getPortfolio() const { return portfolio; }

const unordered_set<string>& User::getWatchlist() const { return watchlist; }
void User::addToWatchlist(const string& ticker) { watchlist.insert(ticker); }
void User::removeFromWatchlist(const string& ticker) { watchlist.erase(ticker); }

// =============================================================================
// ADMIN IMPLEMENTATION
// =============================================================================

Admin::Admin() : Account("", "", "ADMIN") {}

Admin::Admin(const string& username, const string& password)
    : Account(username, password, "ADMIN") {}

void Admin::displayProfile() const {
    cout << "--- Admin Profile ---\n";
    cout << "Username: " << username << "\n";
    cout << "Role:     " << role << " (Full System Privileges)\n";
}
