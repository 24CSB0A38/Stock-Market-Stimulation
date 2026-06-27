/*
 * ============================================================================
 *  Services.cpp — All Service Implementations
 * ============================================================================
 *  Contains: Market, PriceEngine, FileManager, Authentication, Menu
 * ============================================================================
 */

#include "Services.h"
#include <fstream>
#include <queue>
#include <algorithm>

using namespace std;

// =============================================================================
// MARKET IMPLEMENTATION
// =============================================================================

Market::Market() : isOpen(true) {}

bool Market::isMarketOpen() const { return isOpen; }
void Market::setMarketOpen(bool status) { isOpen = status; }

void Market::addStock(const Stock& stock) {
    string tickerUpper = Utils::toUpperCase(stock.getTicker());
    if (stocks.find(tickerUpper) != stocks.end()) {
        throw InvalidStockException("Stock ticker '" + tickerUpper + "' already exists in the market.");
    }
    stocks[tickerUpper] = stock;
}

void Market::removeStock(const string& ticker, const unordered_map<string, User>& users) {
    string tickerUpper = Utils::toUpperCase(ticker);
    auto it = stocks.find(tickerUpper);
    if (it == stocks.end()) {
        throw StockNotFoundException("Ticker '" + tickerUpper + "' not found in market registry.");
    }

    // Business Rule: Cannot remove stock with active shareholders
    for (const auto& pair : users) {
        const auto& holdings = pair.second.getPortfolio().getHoldings();
        if (holdings.find(tickerUpper) != holdings.end()) {
            throw InvalidStockException("Cannot remove stock '" + tickerUpper + "': active shareholders exist.");
        }
    }
    stocks.erase(it);
}

void Market::updateStockPrice(const string& ticker, double newPrice) {
    string tickerUpper = Utils::toUpperCase(ticker);
    auto it = stocks.find(tickerUpper);
    if (it == stocks.end()) {
        throw StockNotFoundException("Ticker '" + tickerUpper + "' not found in market registry.");
    }
    it->second.updatePrice(newPrice);
}

bool Market::hasStock(const string& ticker) const {
    return stocks.find(Utils::toUpperCase(ticker)) != stocks.end();
}

const Stock& Market::getStock(const string& ticker) const {
    string tickerUpper = Utils::toUpperCase(ticker);
    auto it = stocks.find(tickerUpper);
    if (it == stocks.end()) {
        throw StockNotFoundException("Stock ticker '" + tickerUpper + "' not found.");
    }
    return it->second;
}

Stock& Market::getStockWritable(const string& ticker) {
    string tickerUpper = Utils::toUpperCase(ticker);
    auto it = stocks.find(tickerUpper);
    if (it == stocks.end()) {
        throw StockNotFoundException("Stock ticker '" + tickerUpper + "' not found.");
    }
    return it->second;
}

const unordered_map<string, Stock>& Market::getStocks() const { return stocks; }

void Market::displayMarket() const {
    Utils::printHeader("Market Stock Directory");
    cout << "Market Status: " << (isOpen ? "OPEN" : "CLOSED") << "\n\n";

    cout << left
         << setw(8) << "Ticker"
         << setw(22) << "Company Name"
         << setw(12) << "Price"
         << setw(12) << "Change"
         << setw(15) << "Shares Traded" << "\n";
    cout << string(69, '-') << "\n";

    vector<Stock> sortedStocks;
    for (const auto& pair : stocks) {
        sortedStocks.push_back(pair.second);
    }
    sort(sortedStocks.begin(), sortedStocks.end(), [](const Stock& a, const Stock& b) {
        return a.getTicker() < b.getTicker();
    });

    for (const auto& stock : sortedStocks) {
        cout << left
             << setw(8) << stock.getTicker()
             << setw(22) << stock.getCompanyName()
             << setw(12) << Utils::formatCurrency(stock.getCurrentPrice())
             << setw(12) << Utils::formatPercentage(stock.getPriceChangePercent())
             << setw(15) << stock.getTotalSharesTraded() << "\n";
    }
    cout << string(69, '-') << "\n";
}

// Priority Queue Comparators
struct GainerComp {
    bool operator()(const Stock& a, const Stock& b) {
        return a.getPriceChangePercent() < b.getPriceChangePercent();
    }
};

struct LoserComp {
    bool operator()(const Stock& a, const Stock& b) {
        return a.getPriceChangePercent() > b.getPriceChangePercent();
    }
};

struct TradedComp {
    bool operator()(const Stock& a, const Stock& b) {
        return a.getTotalSharesTraded() < b.getTotalSharesTraded();
    }
};

struct VolumeValueComp {
    bool operator()(const Stock& a, const Stock& b) {
        return (a.getTotalSharesTraded() * a.getCurrentPrice()) <
               (b.getTotalSharesTraded() * b.getCurrentPrice());
    }
};

vector<Stock> Market::getTopGainers(size_t limit) const {
    priority_queue<Stock, vector<Stock>, GainerComp> pq;
    for (const auto& pair : stocks) pq.push(pair.second);
    vector<Stock> result;
    while (!pq.empty() && result.size() < limit) { result.push_back(pq.top()); pq.pop(); }
    return result;
}

vector<Stock> Market::getTopLosers(size_t limit) const {
    priority_queue<Stock, vector<Stock>, LoserComp> pq;
    for (const auto& pair : stocks) pq.push(pair.second);
    vector<Stock> result;
    while (!pq.empty() && result.size() < limit) { result.push_back(pq.top()); pq.pop(); }
    return result;
}

vector<Stock> Market::getMostTraded(size_t limit) const {
    priority_queue<Stock, vector<Stock>, TradedComp> pq;
    for (const auto& pair : stocks) pq.push(pair.second);
    vector<Stock> result;
    while (!pq.empty() && result.size() < limit) { result.push_back(pq.top()); pq.pop(); }
    return result;
}

vector<Stock> Market::getHighestVolumeValue(size_t limit) const {
    priority_queue<Stock, vector<Stock>, VolumeValueComp> pq;
    for (const auto& pair : stocks) pq.push(pair.second);
    vector<Stock> result;
    while (!pq.empty() && result.size() < limit) { result.push_back(pq.top()); pq.pop(); }
    return result;
}

// =============================================================================
// PRICE ENGINE IMPLEMENTATION
// =============================================================================

PriceEngine::PriceEngine(double minPercent, double maxPercent)
    : distribution(minPercent, maxPercent) {
    random_device rd;
    generator.seed(rd());
}

void PriceEngine::refreshPrices(Market& market) {
    // Collect tickers first to avoid modifying the map while iterating
    vector<string> tickers;
    for (const auto& pair : market.getStocks()) {
        tickers.push_back(pair.first);
    }

    for (const auto& ticker : tickers) {
        double currentPrice = market.getStock(ticker).getCurrentPrice();
        double percentChange = distribution(generator);
        double newPrice = currentPrice * (1.0 + (percentChange / 100.0));
        if (newPrice < 0.01) newPrice = 0.01;
        market.updateStockPrice(ticker, newPrice);
    }
}

// =============================================================================
// FILE MANAGER IMPLEMENTATION
// =============================================================================

void FileManager::loadMarketData(Market& market, const string& filepath) {
    ifstream file(filepath);
    if (!file.is_open()) return;

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string ticker, name, currentStr, prevStr, initStr, tradedStr, historyStr;

        getline(ss, ticker, ',');
        getline(ss, name, ',');
        getline(ss, currentStr, ',');
        getline(ss, prevStr, ',');
        getline(ss, initStr, ',');
        getline(ss, tradedStr, ',');
        getline(ss, historyStr, ',');

        try {
            Stock stock(ticker, name, stod(currentStr));
            stock.setPreviousPrice(stod(prevStr));
            stock.setInitialPrice(stod(initStr));
            stock.setTotalSharesTraded(stoi(tradedStr));

            stringstream histSS(historyStr);
            string priceVal;
            vector<double> history;
            while (getline(histSS, priceVal, ';')) {
                if (!priceVal.empty()) history.push_back(stod(priceVal));
            }
            stock.setPriceHistory(history);
            market.addStock(stock);
        } catch (...) {
            continue; // Skip malformed records
        }
    }
}

void FileManager::saveMarketData(const Market& market, const string& filepath) {
    ofstream file(filepath);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file for saving stock data: " << filepath << "\n";
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
            if (i < history.size() - 1) file << ";";
        }
        file << "\n";
    }
}

void FileManager::loadUserData(unordered_map<string, User>& users,
                               unordered_map<string, Admin>& admins,
                               const string& filepath) {
    ifstream file(filepath);
    if (!file.is_open()) return;

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string username, password, role, balanceStr, watchlistStr, holdingsStr;

        getline(ss, username, ',');
        getline(ss, password, ',');
        getline(ss, role, ',');
        getline(ss, balanceStr, ',');
        getline(ss, watchlistStr, ',');
        getline(ss, holdingsStr, ',');

        try {
            if (role == "ADMIN") {
                admins[username] = Admin(username, password);
            } else if (role == "USER") {
                User user(username, password, stod(balanceStr));

                // Load Watchlist
                stringstream watchSS(watchlistStr);
                string watchTicker;
                while (getline(watchSS, watchTicker, ';')) {
                    if (!watchTicker.empty()) user.addToWatchlist(watchTicker);
                }

                // Load Portfolio Holdings (format: TICKER:shares:avgPrice;...)
                stringstream holdSS(holdingsStr);
                string holdingItem;
                while (getline(holdSS, holdingItem, ';')) {
                    if (holdingItem.empty()) continue;

                    stringstream itemSS(holdingItem);
                    string hTicker, hSharesStr, hAvgPriceStr;
                    getline(itemSS, hTicker, ':');
                    getline(itemSS, hSharesStr, ':');
                    getline(itemSS, hAvgPriceStr, ':');

                    if (!hTicker.empty() && !hSharesStr.empty() && !hAvgPriceStr.empty()) {
                        user.getPortfolio().buy(hTicker, stoi(hSharesStr), stod(hAvgPriceStr));
                    }
                }
                users[username] = user;
            }
        } catch (...) {
            continue; // Skip malformed records
        }
    }
}

void FileManager::saveUserData(const unordered_map<string, User>& users,
                               const unordered_map<string, Admin>& admins,
                               const string& filepath) {
    ofstream file(filepath);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file for saving user accounts: " << filepath << "\n";
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

        // Watchlist
        const auto& watchlist = user.getWatchlist();
        size_t count = 0;
        for (const auto& ticker : watchlist) {
            file << ticker;
            if (++count < watchlist.size()) file << ";";
        }
        file << ",";

        // Portfolio holdings
        const auto& holdings = user.getPortfolio().getHoldings();
        count = 0;
        for (const auto& hPair : holdings) {
            file << hPair.first << ":" << hPair.second.shares << ":" << hPair.second.averageBuyPrice;
            if (++count < holdings.size()) file << ";";
        }
        file << "\n";
    }
}

vector<Transaction> FileManager::loadTransactions(const string& filepath) {
    vector<Transaction> transactions;
    ifstream file(filepath);
    if (!file.is_open()) return transactions;

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        Transaction tx = Transaction::deserialize(line);
        if (!tx.getTransactionId().empty()) {
            transactions.push_back(tx);
        }
    }
    return transactions;
}

void FileManager::saveTransaction(const Transaction& tx, const string& filepath) {
    ofstream file(filepath, ios::app);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file for appending transaction: " << filepath << "\n";
        return;
    }
    file << tx.serialize() << "\n";
}

void FileManager::saveAllTransactions(const vector<Transaction>& txs, const string& filepath) {
    ofstream file(filepath);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file for saving transactions: " << filepath << "\n";
        return;
    }
    for (const auto& tx : txs) {
        file << tx.serialize() << "\n";
    }
}

// =============================================================================
// AUTHENTICATION IMPLEMENTATION
// =============================================================================

Authentication::Authentication() : currentUser(nullptr) {}

void Authentication::registerUser(const string& username, const string& password, double initialBalance) {
    string userLower = Utils::toUpperCase(username);
    if (users.find(userLower) != users.end() || admins.find(userLower) != admins.end()) {
        throw DuplicateUserException("Username '" + username + "' is already taken.");
    }
    if (initialBalance < 0.0) {
        throw InsufficientBalanceException("Initial deposit cannot be negative.");
    }
    users[userLower] = User(username, password, initialBalance);
}

void Authentication::registerAdmin(const string& username, const string& password) {
    string adminLower = Utils::toUpperCase(username);
    if (users.find(adminLower) != users.end() || admins.find(adminLower) != admins.end()) {
        throw DuplicateUserException("Username '" + username + "' is already taken.");
    }
    admins[adminLower] = Admin(username, password);
}

void Authentication::login(const string& username, const string& password) {
    string nameLower = Utils::toUpperCase(username);

    auto userIt = users.find(nameLower);
    if (userIt != users.end()) {
        if (userIt->second.getPassword() == password) {
            currentUser = &userIt->second;
            return;
        }
        throw AuthenticationFailedException("Incorrect password for username '" + username + "'.");
    }

    auto adminIt = admins.find(nameLower);
    if (adminIt != admins.end()) {
        if (adminIt->second.getPassword() == password) {
            currentUser = &adminIt->second;
            return;
        }
        throw AuthenticationFailedException("Incorrect password for admin username '" + username + "'.");
    }

    throw AuthenticationFailedException("Username '" + username + "' not found.");
}

void Authentication::logout() { currentUser = nullptr; }
Account* Authentication::getCurrentUser() const { return currentUser; }
bool Authentication::isLoggedIn() const { return currentUser != nullptr; }

User& Authentication::getCurrentUserAsUser() {
    if (!currentUser || currentUser->getRole() != "USER") {
        throw AuthenticationFailedException("No active user session found.");
    }
    return *static_cast<User*>(currentUser);
}

const User& Authentication::getCurrentUserAsUser() const {
    if (!currentUser || currentUser->getRole() != "USER") {
        throw AuthenticationFailedException("No active user session found.");
    }
    return *static_cast<const User*>(currentUser);
}

unordered_map<string, User>& Authentication::getUsers() { return users; }
const unordered_map<string, User>& Authentication::getUsers() const { return users; }
unordered_map<string, Admin>& Authentication::getAdmins() { return admins; }
const unordered_map<string, Admin>& Authentication::getAdmins() const { return admins; }

void Authentication::loadUsersFromFile(const string& filepath) {
    FileManager::loadUserData(users, admins, filepath);
}

void Authentication::saveUsersToFile(const string& filepath) const {
    FileManager::saveUserData(users, admins, filepath);
}

pair<string, double> Authentication::getHighestPortfolioValueUser(const Market& market) const {
    string topUser = "None";
    double maxVal = -1.0;
    for (const auto& pair : users) {
        double currentVal = pair.second.getPortfolio().getCurrentValue(market);
        if (currentVal > maxVal) {
            maxVal = currentVal;
            topUser = pair.second.getUsername();
        }
    }
    return {topUser, maxVal < 0.0 ? 0.0 : maxVal};
}

pair<string, double> Authentication::getHighestROIUser(const Market& market) const {
    string topUser = "None";
    double maxROI = -999999.0;
    bool found = false;
    for (const auto& pair : users) {
        double totalInvest = pair.second.getPortfolio().getTotalInvestment();
        if (totalInvest > 0.0) {
            double roi = pair.second.getPortfolio().getROI(market);
            if (roi > maxROI) {
                maxROI = roi;
                topUser = pair.second.getUsername();
                found = true;
            }
        }
    }
    return {topUser, found ? maxROI : 0.0};
}

// =============================================================================
// MENU IMPLEMENTATION
// =============================================================================

Menu::Menu(Market& market, Authentication& auth, PriceEngine& priceEngine,
           vector<Transaction>& transactionLog)
    : market(market), auth(auth), priceEngine(priceEngine), transactionLog(transactionLog) {}

string Menu::generateTransactionId() const {
    static int counter = 0;
    ++counter;
    return "TXN" + to_string(counter) + "-" + Utils::getCurrentTimestamp().substr(0, 10);
}

void Menu::run() {
    FileManager::loadMarketData(market, DATA_STOCKS_FILE);
    auth.loadUsersFromFile(DATA_USERS_FILE);
    transactionLog = FileManager::loadTransactions(DATA_TRANSACTIONS_FILE);

    showWelcomeScreen();

    while (true) {
        if (!auth.isLoggedIn()) {
            Utils::printHeader("Main Menu");
            cout << "  [1] Register\n";
            cout << "  [2] Login\n";
            cout << "  [0] Exit\n";
            cout << "--------------------------------------------------\n";

            int choice = getValidInput<int>("Enter choice: ", 0, 2);

            if (choice == 1) {
                handleRegister();
            } else if (choice == 2) {
                handleLogin();
            } else {
                auth.saveUsersToFile(DATA_USERS_FILE);
                FileManager::saveMarketData(market, DATA_STOCKS_FILE);
                FileManager::saveAllTransactions(transactionLog, DATA_TRANSACTIONS_FILE);
                cout << "\nAll data saved. Goodbye!\n\n";
                return;
            }
        } else {
            string role = auth.getCurrentUser()->getRole();
            if (role == "USER") showUserMenu();
            else if (role == "ADMIN") showAdminMenu();
        }
    }
}

void Menu::showWelcomeScreen() const {
    Utils::clearConsole();
    cout << "\n";
    cout << "  ╔══════════════════════════════════════════════════╗\n";
    cout << "  ║     Stock Market Simulation & Portfolio Mgmt.    ║\n";
    cout << "  ║              C++17  |  STL  |  OOP               ║\n";
    cout << "  ╚══════════════════════════════════════════════════╝\n";
    cout << "\n  Welcome to the Stock Market Simulator.\n";
    cout << "  Simulate real market trades, track profits, manage portfolios.\n\n";
}

void Menu::handleRegister() {
    Utils::printHeader("Register New Account");
    string username, password;

    cout << "Enter username: ";
    getline(cin, username);
    cout << "Enter password: ";
    getline(cin, password);

    double balance = getValidInput<double>("Enter initial deposit ($): ", 0.0, 1e9);

    try {
        auth.registerUser(username, password, balance);
        auth.saveUsersToFile(DATA_USERS_FILE);
        cout << "\n[OK] Account created successfully. Please login.\n";
    } catch (const MarketSystemException& e) {
        cout << "\n[ERROR] " << e.what() << "\n";
    }
}

void Menu::handleLogin() {
    Utils::printHeader("Login");
    string username, password;

    cout << "Enter username: ";
    getline(cin, username);
    cout << "Enter password: ";
    getline(cin, password);

    try {
        auth.login(username, password);
        Utils::clearConsole();
        cout << "\n[OK] Welcome back, " << auth.getCurrentUser()->getUsername() << "!\n";
    } catch (const MarketSystemException& e) {
        cout << "\n[ERROR] " << e.what() << "\n";
    }
}

void Menu::showUserMenu() {
    Utils::printHeader("User Dashboard  [Market: " + string(market.isMarketOpen() ? "OPEN" : "CLOSED") + "]");
    cout << "  Logged in as: " << auth.getCurrentUser()->getUsername() << "\n\n";
    cout << "  -- Market --\n";
    cout << "  [1] View All Stocks\n";
    cout << "  [2] Search Stock\n";
    cout << "  -- Trading --\n";
    cout << "  [3] Buy Stock\n";
    cout << "  [4] Sell Stock\n";
    cout << "  -- Portfolio --\n";
    cout << "  [5] View Portfolio\n";
    cout << "  [6] Transaction History\n";
    cout << "  -- Tools --\n";
    cout << "  [7] Watchlist\n";
    cout << "  [8] Market Analytics\n";
    cout << "  [9] Refresh Market Prices\n";
    cout << "  -- Account --\n";
    cout << "  [10] View Profile\n";
    cout << "  [0] Logout\n";
    cout << "--------------------------------------------------\n";

    int choice = getValidInput<int>("Enter choice: ", 0, 10);

    switch (choice) {
        case 1:  handleMarketDisplay();          break;
        case 2:  handleSearchStock();            break;
        case 3:  handleBuyStock();               break;
        case 4:  handleSellStock();              break;
        case 5:  handleViewPortfolio();          break;
        case 6:  handleViewTransactionHistory(); break;
        case 7:  handleWatchlist();              break;
        case 8:  handleAnalytics();              break;
        case 9:  handleRefreshPrices();          break;
        case 10: handleViewProfile();            break;
        case 0:
            auth.saveUsersToFile(DATA_USERS_FILE);
            FileManager::saveAllTransactions(transactionLog, DATA_TRANSACTIONS_FILE);
            auth.logout();
            cout << "\n[OK] Logged out successfully.\n";
            break;
    }
}

void Menu::showAdminMenu() {
    Utils::printHeader("Admin Dashboard");
    cout << "  Logged in as: " << auth.getCurrentUser()->getUsername() << " [ADMIN]\n\n";
    cout << "  [1] View Market\n";
    cout << "  [2] Add Stock\n";
    cout << "  [3] Remove Stock\n";
    cout << "  [4] Update Stock Price\n";
    cout << "  [5] Market Analytics\n";
    cout << "  [6] Toggle Market Status (OPEN/CLOSED)\n";
    cout << "  [7] View Profile\n";
    cout << "  [0] Logout\n";
    cout << "--------------------------------------------------\n";

    int choice = getValidInput<int>("Enter choice: ", 0, 7);

    switch (choice) {
        case 1: handleMarketDisplay();       break;
        case 2: handleAddStock();            break;
        case 3: handleRemoveStock();         break;
        case 4: handleUpdateStockPrice();    break;
        case 5: handleAdminAnalytics();      break;
        case 6: handleMarketStatusToggle();  break;
        case 7: handleAdminProfile();        break;
        case 0:
            auth.saveUsersToFile(DATA_USERS_FILE);
            FileManager::saveMarketData(market, DATA_STOCKS_FILE);
            FileManager::saveAllTransactions(transactionLog, DATA_TRANSACTIONS_FILE);
            auth.logout();
            cout << "\n[OK] Admin logged out.\n";
            break;
    }
}

void Menu::handleMarketDisplay() const { market.displayMarket(); }

void Menu::handleSearchStock() const {
    string ticker;
    cout << "\nEnter stock ticker to search: ";
    cin >> ticker;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    ticker = Utils::toUpperCase(ticker);

    try {
        const Stock& stock = market.getStock(ticker);
        Utils::printHeader("Stock Details: " + ticker);
        cout << "  Ticker:          " << stock.getTicker() << "\n";
        cout << "  Company:         " << stock.getCompanyName() << "\n";
        cout << "  Current Price:   " << Utils::formatCurrency(stock.getCurrentPrice()) << "\n";
        cout << "  Previous Price:  " << Utils::formatCurrency(stock.getPreviousPrice()) << "\n";
        cout << "  Initial Price:   " << Utils::formatCurrency(stock.getInitialPrice()) << "\n";
        cout << "  Daily Change:    " << Utils::formatPercentage(stock.getPriceChangePercent()) << "\n";
        cout << "  All-time Change: " << Utils::formatPercentage(stock.getPriceChangeFromInitialPercent()) << "\n";
        cout << "  Shares Traded:   " << stock.getTotalSharesTraded() << "\n";
        cout << "  Price History:   ";
        const auto& hist = stock.getPriceHistory();
        size_t start = hist.size() > 5 ? hist.size() - 5 : 0;
        for (size_t i = start; i < hist.size(); ++i) {
            cout << Utils::formatCurrency(hist[i]);
            if (i < hist.size() - 1) cout << " -> ";
        }
        cout << "\n";
    } catch (const MarketSystemException& e) {
        cout << "\n[ERROR] " << e.what() << "\n";
    }
}

void Menu::handleBuyStock() {
    if (!market.isMarketOpen()) {
        cout << "\n[BLOCKED] Market is currently CLOSED. Trades are not allowed.\n";
        return;
    }

    string ticker;
    cout << "\nEnter stock ticker to buy: ";
    cin >> ticker;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    ticker = Utils::toUpperCase(ticker);

    try {
        const Stock& stock = market.getStock(ticker);
        User& user = auth.getCurrentUserAsUser();

        cout << "  Stock: " << stock.getCompanyName()
             << " | Current Price: " << Utils::formatCurrency(stock.getCurrentPrice())
             << " | Your Balance: " << Utils::formatCurrency(user.getBalance()) << "\n";

        int quantity = getValidInput<int>("Enter number of shares to buy: ", 1, 100000);
        double totalCost = quantity * stock.getCurrentPrice();

        cout << "  Total Cost: " << Utils::formatCurrency(totalCost) << "\n";
        cout << "  Confirm purchase? [1=Yes / 0=No]: ";
        int confirm = getValidInput<int>("", 0, 1);
        if (confirm != 1) { cout << "  Purchase cancelled.\n"; return; }

        user.debitBalance(totalCost);
        user.getPortfolio().buy(ticker, quantity, stock.getCurrentPrice());
        market.getStockWritable(ticker).addSharesTraded(quantity);

        Transaction tx(generateTransactionId(), user.getUsername(), ticker, "BUY", quantity,
                       stock.getCurrentPrice(), Utils::getCurrentTimestamp());
        transactionLog.push_back(tx);
        FileManager::saveTransaction(tx, DATA_TRANSACTIONS_FILE);
        auth.saveUsersToFile(DATA_USERS_FILE);

        cout << "\n[OK] Bought " << quantity << " shares of " << ticker
             << " at " << Utils::formatCurrency(stock.getCurrentPrice()) << "\n";
        cout << "  Remaining Balance: " << Utils::formatCurrency(user.getBalance()) << "\n";
    } catch (const MarketSystemException& e) {
        cout << "\n[ERROR] " << e.what() << "\n";
    }
}

void Menu::handleSellStock() {
    if (!market.isMarketOpen()) {
        cout << "\n[BLOCKED] Market is currently CLOSED. Trades are not allowed.\n";
        return;
    }

    User& user = auth.getCurrentUserAsUser();
    const auto& holdings = user.getPortfolio().getHoldings();

    if (holdings.empty()) {
        cout << "\n[INFO] Your portfolio is empty. Nothing to sell.\n";
        return;
    }

    string ticker;
    cout << "\nEnter stock ticker to sell: ";
    cin >> ticker;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    ticker = Utils::toUpperCase(ticker);

    try {
        const Stock& stock = market.getStock(ticker);
        auto it = holdings.find(ticker);
        if (it == holdings.end()) {
            cout << "\n[ERROR] You do not own any shares of " << ticker << ".\n";
            return;
        }

        cout << "  Stock: " << stock.getCompanyName()
             << " | Current Price: " << Utils::formatCurrency(stock.getCurrentPrice())
             << " | Owned Shares: " << it->second.shares << "\n";

        int quantity = getValidInput<int>("Enter number of shares to sell: ", 1, it->second.shares);
        double proceeds = quantity * stock.getCurrentPrice();
        double profitLoss = (stock.getCurrentPrice() - it->second.averageBuyPrice) * quantity;

        cout << "  Total Proceeds: " << Utils::formatCurrency(proceeds) << "\n";
        cout << "  Est. P&L on this sale: " << Utils::formatCurrency(profitLoss) << "\n";
        cout << "  Confirm sale? [1=Yes / 0=No]: ";
        int confirm = getValidInput<int>("", 0, 1);
        if (confirm != 1) { cout << "  Sale cancelled.\n"; return; }

        user.getPortfolio().sell(ticker, quantity);
        user.creditBalance(proceeds);
        market.getStockWritable(ticker).addSharesTraded(quantity);

        Transaction tx(generateTransactionId(), user.getUsername(), ticker, "SELL", quantity,
                       stock.getCurrentPrice(), Utils::getCurrentTimestamp());
        transactionLog.push_back(tx);
        FileManager::saveTransaction(tx, DATA_TRANSACTIONS_FILE);
        auth.saveUsersToFile(DATA_USERS_FILE);

        cout << "\n[OK] Sold " << quantity << " shares of " << ticker
             << " at " << Utils::formatCurrency(stock.getCurrentPrice()) << "\n";
        cout << "  New Balance: " << Utils::formatCurrency(user.getBalance()) << "\n";
    } catch (const MarketSystemException& e) {
        cout << "\n[ERROR] " << e.what() << "\n";
    }
}

void Menu::handleViewPortfolio() const {
    const User& user = auth.getCurrentUserAsUser();
    const auto& holdings = user.getPortfolio().getHoldings();

    Utils::printHeader("Portfolio  |  " + user.getUsername());
    cout << "  Wallet Balance:    " << Utils::formatCurrency(user.getBalance()) << "\n\n";

    if (holdings.empty()) {
        cout << "  [INFO] Your portfolio is empty. Buy some stocks to get started.\n";
        return;
    }

    cout << left
         << setw(8) << "Ticker"
         << setw(8) << "Shares"
         << setw(14) << "Avg Buy Price"
         << setw(14) << "Market Price"
         << setw(14) << "Investment"
         << setw(14) << "Curr. Value"
         << setw(12) << "Net P&L" << "\n";
    cout << string(84, '-') << "\n";

    double totalInvest = 0.0, totalValue = 0.0;

    for (const auto& pair : holdings) {
        const Holding& h = pair.second;
        double mktPrice = market.getStock(pair.first).getCurrentPrice();
        double invest = h.shares * h.averageBuyPrice;
        double currVal = h.shares * mktPrice;
        double pnl = currVal - invest;

        totalInvest += invest;
        totalValue += currVal;

        cout << left
             << setw(8) << h.ticker
             << setw(8) << h.shares
             << setw(14) << Utils::formatCurrency(h.averageBuyPrice)
             << setw(14) << Utils::formatCurrency(mktPrice)
             << setw(14) << Utils::formatCurrency(invest)
             << setw(14) << Utils::formatCurrency(currVal)
             << setw(12) << Utils::formatCurrency(pnl) << "\n";
    }

    cout << string(84, '-') << "\n";
    double netPnL = totalValue - totalInvest;
    double roi = (totalInvest > 0.0) ? ((netPnL / totalInvest) * 100.0) : 0.0;

    cout << "\n  PORTFOLIO SUMMARY\n";
    cout << "  Total Investment:  " << Utils::formatCurrency(totalInvest) << "\n";
    cout << "  Current Value:     " << Utils::formatCurrency(totalValue) << "\n";
    cout << "  Net Profit/Loss:   " << Utils::formatCurrency(netPnL) << "\n";
    cout << "  ROI:               " << Utils::formatPercentage(roi) << "\n";
}

void Menu::handleViewTransactionHistory() const {
    const User& user = auth.getCurrentUserAsUser();

    Utils::printHeader("Transaction History  |  " + user.getUsername());

    bool found = false;
    int count = 0;

    cout << left
         << setw(22) << "Transaction ID"
         << setw(8) << "Ticker"
         << setw(6) << "Type"
         << setw(8) << "Shares"
         << setw(12) << "Price"
         << setw(22) << "Timestamp" << "\n";
    cout << string(78, '-') << "\n";

    for (const auto& tx : transactionLog) {
        if (tx.getUsername() == user.getUsername()) {
            cout << left
                 << setw(22) << tx.getTransactionId()
                 << setw(8) << tx.getTicker()
                 << setw(6) << tx.getType()
                 << setw(8) << tx.getQuantity()
                 << setw(12) << Utils::formatCurrency(tx.getPrice())
                 << setw(22) << tx.getTimestamp() << "\n";
            found = true;
            count++;
        }
    }

    if (!found) {
        cout << "  [INFO] No transactions found for your account.\n";
    }

    cout << string(78, '-') << "\n";
    cout << "  Total Transactions: " << count << "\n";
}

void Menu::handleWatchlist() {
    User& user = auth.getCurrentUserAsUser();

    while (true) {
        Utils::printHeader("Watchlist  |  " + user.getUsername());
        const auto& watchlist = user.getWatchlist();

        if (watchlist.empty()) {
            cout << "  [INFO] Watchlist is empty.\n";
        } else {
            cout << "  Tracked Stocks:\n";
            for (const auto& ticker : watchlist) {
                try {
                    const Stock& s = market.getStock(ticker);
                    cout << "    " << left << setw(8) << ticker
                         << Utils::formatCurrency(s.getCurrentPrice())
                         << "  " << Utils::formatPercentage(s.getPriceChangePercent()) << "\n";
                } catch (...) {
                    cout << "    " << ticker << " (price unavailable)\n";
                }
            }
        }

        cout << "\n  [1] Add Stock to Watchlist\n";
        cout << "  [2] Remove Stock from Watchlist\n";
        cout << "  [0] Back\n";
        cout << "--------------------------------------------------\n";

        int choice = getValidInput<int>("Enter choice: ", 0, 2);
        if (choice == 0) break;

        string ticker;
        cout << "Enter stock ticker: ";
        cin >> ticker;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        ticker = Utils::toUpperCase(ticker);

        if (choice == 1) {
            if (!market.hasStock(ticker)) {
                cout << "\n[ERROR] Ticker '" << ticker << "' not found in market.\n";
            } else {
                user.addToWatchlist(ticker);
                auth.saveUsersToFile(DATA_USERS_FILE);
                cout << "\n[OK] " << ticker << " added to watchlist.\n";
            }
        } else if (choice == 2) {
            user.removeFromWatchlist(ticker);
            auth.saveUsersToFile(DATA_USERS_FILE);
            cout << "\n[OK] " << ticker << " removed from watchlist.\n";
        }
    }
}

void Menu::handleAnalytics() const {
    displayAnalyticsResults();
    displayUserAnalytics();
}

void Menu::displayAnalyticsResults() const {
    Utils::printHeader("Market Analytics");

    auto printList = [&](const string& title, const vector<Stock>& stocks) {
        cout << "\n  " << title << "\n";
        cout << "  " << string(50, '-') << "\n";
        if (stocks.empty()) { cout << "  No data available.\n"; return; }
        for (size_t i = 0; i < stocks.size(); ++i) {
            cout << "  " << (i + 1) << ". "
                 << left << setw(8) << stocks[i].getTicker()
                 << setw(20) << stocks[i].getCompanyName()
                 << Utils::formatCurrency(stocks[i].getCurrentPrice()) << "  "
                 << Utils::formatPercentage(stocks[i].getPriceChangePercent()) << "\n";
        }
    };

    printList("Top Gainers",        market.getTopGainers(5));
    printList("Top Losers",         market.getTopLosers(5));
    printList("Most Traded",        market.getMostTraded(5));
    printList("Highest Volume ($)", market.getHighestVolumeValue(5));
}

void Menu::displayUserAnalytics() const {
    Utils::printHeader("Cross-User Analytics");

    auto [topValueUser, topValue] = auth.getHighestPortfolioValueUser(market);
    auto [topROIUser, topROI] = auth.getHighestROIUser(market);

    cout << "  Highest Portfolio Value:\n";
    cout << "    " << topValueUser << "  ->  " << Utils::formatCurrency(topValue) << "\n";
    cout << "\n  Highest ROI:\n";
    cout << "    " << topROIUser << "  ->  " << Utils::formatPercentage(topROI) << "\n";
}

void Menu::handleRefreshPrices() {
    priceEngine.refreshPrices(market);
    FileManager::saveMarketData(market, DATA_STOCKS_FILE);
    cout << "\n[OK] Market prices refreshed. Analytics updated.\n";
    market.displayMarket();
}

void Menu::handleAdminProfile() const { auth.getCurrentUser()->displayProfile(); }

void Menu::handleAddStock() {
    Utils::printHeader("Add New Stock");
    string ticker, name;

    cout << "Enter stock ticker (e.g. AAPL): ";
    cin >> ticker;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    ticker = Utils::toUpperCase(ticker);

    cout << "Enter company name: ";
    getline(cin, name);

    double price = getValidInput<double>("Enter initial price ($): ", 0.01, 1e7);

    try {
        Stock newStock(ticker, name, price);
        market.addStock(newStock);
        FileManager::saveMarketData(market, DATA_STOCKS_FILE);
        cout << "\n[OK] Stock '" << ticker << "' listed on market at " << Utils::formatCurrency(price) << ".\n";
    } catch (const MarketSystemException& e) {
        cout << "\n[ERROR] " << e.what() << "\n";
    }
}

void Menu::handleRemoveStock() {
    Utils::printHeader("Remove Stock");
    string ticker;

    cout << "Enter stock ticker to remove: ";
    cin >> ticker;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    ticker = Utils::toUpperCase(ticker);

    try {
        market.removeStock(ticker, auth.getUsers());
        FileManager::saveMarketData(market, DATA_STOCKS_FILE);
        cout << "\n[OK] Stock '" << ticker << "' removed from market.\n";
    } catch (const MarketSystemException& e) {
        cout << "\n[ERROR] " << e.what() << "\n";
    }
}

void Menu::handleUpdateStockPrice() {
    Utils::printHeader("Update Stock Price");
    string ticker;

    cout << "Enter stock ticker: ";
    cin >> ticker;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    ticker = Utils::toUpperCase(ticker);

    try {
        const Stock& stock = market.getStock(ticker);
        cout << "  Current price: " << Utils::formatCurrency(stock.getCurrentPrice()) << "\n";
        double newPrice = getValidInput<double>("Enter new price ($): ", 0.01, 1e7);

        market.updateStockPrice(ticker, newPrice);
        FileManager::saveMarketData(market, DATA_STOCKS_FILE);
        cout << "\n[OK] Price of '" << ticker << "' updated to " << Utils::formatCurrency(newPrice) << ".\n";
    } catch (const MarketSystemException& e) {
        cout << "\n[ERROR] " << e.what() << "\n";
    }
}

void Menu::handleAdminAnalytics() const {
    displayAnalyticsResults();
    displayUserAnalytics();
}

void Menu::handleMarketStatusToggle() {
    bool newStatus = !market.isMarketOpen();
    market.setMarketOpen(newStatus);
    cout << "\n[OK] Market is now " << (newStatus ? "OPEN" : "CLOSED") << ".\n";
}

void Menu::handleViewProfile() const {
    Utils::printHeader("Profile");
    auth.getCurrentUser()->displayProfile();
}
