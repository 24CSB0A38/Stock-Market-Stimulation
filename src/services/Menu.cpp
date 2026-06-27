#include "services/Menu.h"
#include "services/FileManager.h"
#include "utils/Exceptions.h"
#include "utils/Helper.h"
#include "utils/Templates.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <limits>

namespace StockMarket {

    Menu::Menu(Market& market, Authentication& auth, PriceEngine& priceEngine,
               std::vector<Transaction>& transactionLog)
        : market(market), auth(auth), priceEngine(priceEngine), transactionLog(transactionLog) {}

    // =========================================================================
    // TRANSACTION ID GENERATOR
    // =========================================================================

    std::string Menu::generateTransactionId() const {
        // Transaction count becomes part of the ID. Simple, reproducible, and unique per session.
        static int counter = 0;
        ++counter;
        return "TXN" + std::to_string(counter) + "-" + Utils::getCurrentTimestamp().substr(0, 10);
    }

    // =========================================================================
    // MAIN ENTRY POINT
    // =========================================================================

    void Menu::run() {
        // Load all persisted data on startup
        FileManager::loadMarketData(market, DATA_STOCKS_FILE);
        auth.loadUsersFromFile(DATA_USERS_FILE);
        transactionLog = FileManager::loadTransactions(DATA_TRANSACTIONS_FILE);

        showWelcomeScreen();

        while (true) {
            if (!auth.isLoggedIn()) {
                Utils::printHeader("Main Menu");
                std::cout << "  [1] Register\n";
                std::cout << "  [2] Login\n";
                std::cout << "  [0] Exit\n";
                std::cout << "--------------------------------------------------\n";

                int choice = getValidInput<int>("Enter choice: ", 0, 2);

                if (choice == 1) {
                    handleRegister();
                } else if (choice == 2) {
                    handleLogin();
                } else {
                    // Save everything before exiting
                    auth.saveUsersToFile(DATA_USERS_FILE);
                    FileManager::saveMarketData(market, DATA_STOCKS_FILE);
                    FileManager::saveAllTransactions(transactionLog, DATA_TRANSACTIONS_FILE);
                    std::cout << "\nAll data saved. Goodbye!\n\n";
                    return;
                }
            } else {
                std::string role = auth.getCurrentUser()->getRole();
                if (role == "USER") {
                    showUserMenu();
                } else if (role == "ADMIN") {
                    showAdminMenu();
                }
            }
        }
    }

    // =========================================================================
    // WELCOME SCREEN
    // =========================================================================

    void Menu::showWelcomeScreen() const {
        Utils::clearConsole();
        std::cout << "\n";
        std::cout << "  ╔══════════════════════════════════════════════════╗\n";
        std::cout << "  ║     Stock Market Simulation & Portfolio Mgmt.    ║\n";
        std::cout << "  ║              C++17  |  STL  |  OOP               ║\n";
        std::cout << "  ╚══════════════════════════════════════════════════╝\n";
        std::cout << "\n  Welcome to the Stock Market Simulator.\n";
        std::cout << "  Simulate real market trades, track profits, manage portfolios.\n\n";
    }

    // =========================================================================
    // AUTH HANDLERS
    // =========================================================================

    void Menu::handleRegister() {
        Utils::printHeader("Register New Account");
        std::string username, password;

        std::cout << "Enter username: ";
        std::getline(std::cin, username);
        std::cout << "Enter password: ";
        std::getline(std::cin, password);

        double balance = getValidInput<double>("Enter initial deposit ($): ", 0.0, 1e9);

        try {
            auth.registerUser(username, password, balance);
            auth.saveUsersToFile(DATA_USERS_FILE);
            std::cout << "\n[OK] Account created successfully. Please login.\n";
        } catch (const MarketSystemException& e) {
            std::cout << "\n[ERROR] " << e.what() << "\n";
        }
    }

    void Menu::handleLogin() {
        Utils::printHeader("Login");
        std::string username, password;

        std::cout << "Enter username: ";
        std::getline(std::cin, username);
        std::cout << "Enter password: ";
        std::getline(std::cin, password);

        try {
            auth.login(username, password);
            Utils::clearConsole();
            std::cout << "\n[OK] Welcome back, " << auth.getCurrentUser()->getUsername() << "!\n";
        } catch (const MarketSystemException& e) {
            std::cout << "\n[ERROR] " << e.what() << "\n";
        }
    }

    // =========================================================================
    // USER MENU
    // =========================================================================

    void Menu::showUserMenu() {
        Utils::printHeader("User Dashboard  [Market: " + std::string(market.isMarketOpen() ? "OPEN" : "CLOSED") + "]");
        std::cout << "  Logged in as: " << auth.getCurrentUser()->getUsername() << "\n\n";
        std::cout << "  -- Market --\n";
        std::cout << "  [1] View All Stocks\n";
        std::cout << "  [2] Search Stock\n";
        std::cout << "  -- Trading --\n";
        std::cout << "  [3] Buy Stock\n";
        std::cout << "  [4] Sell Stock\n";
        std::cout << "  -- Portfolio --\n";
        std::cout << "  [5] View Portfolio\n";
        std::cout << "  [6] Transaction History\n";
        std::cout << "  -- Tools --\n";
        std::cout << "  [7] Watchlist\n";
        std::cout << "  [8] Market Analytics\n";
        std::cout << "  [9] Refresh Market Prices\n";
        std::cout << "  -- Account --\n";
        std::cout << "  [10] View Profile\n";
        std::cout << "  [0] Logout\n";
        std::cout << "--------------------------------------------------\n";

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
                std::cout << "\n[OK] Logged out successfully.\n";
                break;
        }
    }

    // =========================================================================
    // ADMIN MENU
    // =========================================================================

    void Menu::showAdminMenu() {
        Utils::printHeader("Admin Dashboard");
        std::cout << "  Logged in as: " << auth.getCurrentUser()->getUsername() << " [ADMIN]\n\n";
        std::cout << "  [1] View Market\n";
        std::cout << "  [2] Add Stock\n";
        std::cout << "  [3] Remove Stock\n";
        std::cout << "  [4] Update Stock Price\n";
        std::cout << "  [5] Market Analytics\n";
        std::cout << "  [6] Toggle Market Status (OPEN/CLOSED)\n";
        std::cout << "  [7] View Profile\n";
        std::cout << "  [0] Logout\n";
        std::cout << "--------------------------------------------------\n";

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
                std::cout << "\n[OK] Admin logged out.\n";
                break;
        }
    }

    // =========================================================================
    // MARKET DISPLAY & SEARCH
    // =========================================================================

    void Menu::handleMarketDisplay() const {
        market.displayMarket();
    }

    void Menu::handleSearchStock() const {
        std::string ticker;
        std::cout << "\nEnter stock ticker to search: ";
        std::cin >> ticker;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        ticker = Utils::toUpperCase(ticker);

        try {
            const Stock& stock = market.getStock(ticker);
            Utils::printHeader("Stock Details: " + ticker);
            std::cout << "  Ticker:          " << stock.getTicker() << "\n";
            std::cout << "  Company:         " << stock.getCompanyName() << "\n";
            std::cout << "  Current Price:   " << Utils::formatCurrency(stock.getCurrentPrice()) << "\n";
            std::cout << "  Previous Price:  " << Utils::formatCurrency(stock.getPreviousPrice()) << "\n";
            std::cout << "  Initial Price:   " << Utils::formatCurrency(stock.getInitialPrice()) << "\n";
            std::cout << "  Daily Change:    " << Utils::formatPercentage(stock.getPriceChangePercent()) << "\n";
            std::cout << "  All-time Change: " << Utils::formatPercentage(stock.getPriceChangeFromInitialPercent()) << "\n";
            std::cout << "  Shares Traded:   " << stock.getTotalSharesTraded() << "\n";
            std::cout << "  Price History:   ";
            const auto& hist = stock.getPriceHistory();
            size_t start = hist.size() > 5 ? hist.size() - 5 : 0;
            for (size_t i = start; i < hist.size(); ++i) {
                std::cout << Utils::formatCurrency(hist[i]);
                if (i < hist.size() - 1) std::cout << " -> ";
            }
            std::cout << "\n";
        } catch (const MarketSystemException& e) {
            std::cout << "\n[ERROR] " << e.what() << "\n";
        }
    }

    // =========================================================================
    // TRADING: BUY
    // =========================================================================

    void Menu::handleBuyStock() {
        if (!market.isMarketOpen()) {
            std::cout << "\n[BLOCKED] Market is currently CLOSED. Trades are not allowed.\n";
            return;
        }

        std::string ticker;
        std::cout << "\nEnter stock ticker to buy: ";
        std::cin >> ticker;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        ticker = Utils::toUpperCase(ticker);

        try {
            const Stock& stock = market.getStock(ticker);
            User& user = auth.getCurrentUserAsUser();

            std::cout << "  Stock: " << stock.getCompanyName()
                      << " | Current Price: " << Utils::formatCurrency(stock.getCurrentPrice())
                      << " | Your Balance: " << Utils::formatCurrency(user.getBalance()) << "\n";

            int quantity = getValidInput<int>("Enter number of shares to buy: ", 1, 100000);
            double totalCost = quantity * stock.getCurrentPrice();

            std::cout << "  Total Cost: " << Utils::formatCurrency(totalCost) << "\n";
            std::cout << "  Confirm purchase? [1=Yes / 0=No]: ";
            int confirm = getValidInput<int>("", 0, 1);
            if (confirm != 1) {
                std::cout << "  Purchase cancelled.\n";
                return;
            }

            // Business logic: debit wallet first (throws if balance insufficient)
            user.debitBalance(totalCost);

            // Update portfolio
            user.getPortfolio().buy(ticker, quantity, stock.getCurrentPrice());

            // Update market trade volume
            market.getStockWritable(ticker).addSharesTraded(quantity);

            // Record transaction
            Transaction tx(generateTransactionId(), user.getUsername(), ticker, "BUY", quantity,
                           stock.getCurrentPrice(), Utils::getCurrentTimestamp());
            transactionLog.push_back(tx);
            FileManager::saveTransaction(tx, DATA_TRANSACTIONS_FILE);
            auth.saveUsersToFile(DATA_USERS_FILE);

            std::cout << "\n[OK] Bought " << quantity << " shares of " << ticker
                      << " at " << Utils::formatCurrency(stock.getCurrentPrice()) << "\n";
            std::cout << "  Remaining Balance: " << Utils::formatCurrency(user.getBalance()) << "\n";

        } catch (const MarketSystemException& e) {
            std::cout << "\n[ERROR] " << e.what() << "\n";
        }
    }

    // =========================================================================
    // TRADING: SELL
    // =========================================================================

    void Menu::handleSellStock() {
        if (!market.isMarketOpen()) {
            std::cout << "\n[BLOCKED] Market is currently CLOSED. Trades are not allowed.\n";
            return;
        }

        User& user = auth.getCurrentUserAsUser();
        const auto& holdings = user.getPortfolio().getHoldings();

        if (holdings.empty()) {
            std::cout << "\n[INFO] Your portfolio is empty. Nothing to sell.\n";
            return;
        }

        std::string ticker;
        std::cout << "\nEnter stock ticker to sell: ";
        std::cin >> ticker;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        ticker = Utils::toUpperCase(ticker);

        try {
            const Stock& stock = market.getStock(ticker);
            auto it = holdings.find(ticker);
            if (it == holdings.end()) {
                std::cout << "\n[ERROR] You do not own any shares of " << ticker << ".\n";
                return;
            }

            std::cout << "  Stock: " << stock.getCompanyName()
                      << " | Current Price: " << Utils::formatCurrency(stock.getCurrentPrice())
                      << " | Owned Shares: " << it->second.shares << "\n";

            int quantity = getValidInput<int>("Enter number of shares to sell: ", 1, it->second.shares);
            double proceeds = quantity * stock.getCurrentPrice();
            double profitLoss = (stock.getCurrentPrice() - it->second.averageBuyPrice) * quantity;

            std::cout << "  Total Proceeds: " << Utils::formatCurrency(proceeds) << "\n";
            std::cout << "  Est. P&L on this sale: " << Utils::formatCurrency(profitLoss) << "\n";
            std::cout << "  Confirm sale? [1=Yes / 0=No]: ";
            int confirm = getValidInput<int>("", 0, 1);
            if (confirm != 1) {
                std::cout << "  Sale cancelled.\n";
                return;
            }

            // Update portfolio
            user.getPortfolio().sell(ticker, quantity);

            // Credit proceeds to wallet
            user.creditBalance(proceeds);

            // Update market volume
            market.getStockWritable(ticker).addSharesTraded(quantity);

            // Record transaction
            Transaction tx(generateTransactionId(), user.getUsername(), ticker, "SELL", quantity,
                           stock.getCurrentPrice(), Utils::getCurrentTimestamp());
            transactionLog.push_back(tx);
            FileManager::saveTransaction(tx, DATA_TRANSACTIONS_FILE);
            auth.saveUsersToFile(DATA_USERS_FILE);

            std::cout << "\n[OK] Sold " << quantity << " shares of " << ticker
                      << " at " << Utils::formatCurrency(stock.getCurrentPrice()) << "\n";
            std::cout << "  New Balance: " << Utils::formatCurrency(user.getBalance()) << "\n";

        } catch (const MarketSystemException& e) {
            std::cout << "\n[ERROR] " << e.what() << "\n";
        }
    }

    // =========================================================================
    // PORTFOLIO
    // =========================================================================

    void Menu::handleViewPortfolio() const {
        const User& user = auth.getCurrentUserAsUser();
        const auto& holdings = user.getPortfolio().getHoldings();

        Utils::printHeader("Portfolio  |  " + user.getUsername());
        std::cout << "  Wallet Balance:    " << Utils::formatCurrency(user.getBalance()) << "\n\n";

        if (holdings.empty()) {
            std::cout << "  [INFO] Your portfolio is empty. Buy some stocks to get started.\n";
            return;
        }

        std::cout << std::left
                  << std::setw(8)  << "Ticker"
                  << std::setw(8)  << "Shares"
                  << std::setw(14) << "Avg Buy Price"
                  << std::setw(14) << "Market Price"
                  << std::setw(14) << "Investment"
                  << std::setw(14) << "Curr. Value"
                  << std::setw(12) << "Net P&L"
                  << "\n";
        std::cout << std::string(84, '-') << "\n";

        double totalInvest = 0.0, totalValue = 0.0;

        for (const auto& pair : holdings) {
            const Holding& h = pair.second;
            double mktPrice = market.getStock(pair.first).getCurrentPrice();
            double invest   = h.shares * h.averageBuyPrice;
            double currVal  = h.shares * mktPrice;
            double pnl      = currVal - invest;

            totalInvest += invest;
            totalValue  += currVal;

            std::cout << std::left
                      << std::setw(8)  << h.ticker
                      << std::setw(8)  << h.shares
                      << std::setw(14) << Utils::formatCurrency(h.averageBuyPrice)
                      << std::setw(14) << Utils::formatCurrency(mktPrice)
                      << std::setw(14) << Utils::formatCurrency(invest)
                      << std::setw(14) << Utils::formatCurrency(currVal)
                      << std::setw(12) << Utils::formatCurrency(pnl)
                      << "\n";
        }

        std::cout << std::string(84, '-') << "\n";

        double netPnL = totalValue - totalInvest;
        double roi    = (totalInvest > 0.0) ? ((netPnL / totalInvest) * 100.0) : 0.0;

        std::cout << "\n  PORTFOLIO SUMMARY\n";
        std::cout << "  Total Investment:  " << Utils::formatCurrency(totalInvest) << "\n";
        std::cout << "  Current Value:     " << Utils::formatCurrency(totalValue)  << "\n";
        std::cout << "  Net Profit/Loss:   " << Utils::formatCurrency(netPnL)      << "\n";
        std::cout << "  ROI:               " << Utils::formatPercentage(roi)       << "\n";
    }

    // =========================================================================
    // TRANSACTION HISTORY
    // =========================================================================

    void Menu::handleViewTransactionHistory() const {
        const User& user = auth.getCurrentUserAsUser();

        Utils::printHeader("Transaction History  |  " + user.getUsername());

        // Filter transactions belonging to this user
        bool found = false;
        int count = 0;
        
        std::cout << std::left
                  << std::setw(22) << "Transaction ID"
                  << std::setw(8)  << "Ticker"
                  << std::setw(6)  << "Type"
                  << std::setw(8)  << "Shares"
                  << std::setw(12) << "Price"
                  << std::setw(22) << "Timestamp"
                  << "\n";
        std::cout << std::string(78, '-') << "\n";
        
        for (const auto& tx : transactionLog) {
            if (tx.getUsername() == user.getUsername()) {
                std::cout << std::left
                          << std::setw(22) << tx.getTransactionId()
                          << std::setw(8)  << tx.getTicker()
                          << std::setw(6)  << tx.getType()
                          << std::setw(8)  << tx.getQuantity()
                          << std::setw(12) << Utils::formatCurrency(tx.getPrice())
                          << std::setw(22) << tx.getTimestamp()
                          << "\n";
                found = true;
                count++;
            }
        }
        
        if (!found) {
            std::cout << "  [INFO] No transactions found for your account.\n";
        }
        
        std::cout << std::string(78, '-') << "\n";
        std::cout << "  Total Transactions: " << count << "\n";
    }

    // =========================================================================
    // WATCHLIST
    // =========================================================================

    void Menu::handleWatchlist() {
        User& user = auth.getCurrentUserAsUser();

        while (true) {
            Utils::printHeader("Watchlist  |  " + user.getUsername());
            const auto& watchlist = user.getWatchlist();

            if (watchlist.empty()) {
                std::cout << "  [INFO] Watchlist is empty.\n";
            } else {
                std::cout << "  Tracked Stocks:\n";
                for (const auto& ticker : watchlist) {
                    try {
                        const Stock& s = market.getStock(ticker);
                        std::cout << "    " << std::left << std::setw(8) << ticker
                                  << Utils::formatCurrency(s.getCurrentPrice())
                                  << "  " << Utils::formatPercentage(s.getPriceChangePercent()) << "\n";
                    } catch (...) {
                        std::cout << "    " << ticker << " (price unavailable)\n";
                    }
                }
            }

            std::cout << "\n  [1] Add Stock to Watchlist\n";
            std::cout << "  [2] Remove Stock from Watchlist\n";
            std::cout << "  [0] Back\n";
            std::cout << "--------------------------------------------------\n";

            int choice = getValidInput<int>("Enter choice: ", 0, 2);
            if (choice == 0) break;

            std::string ticker;
            std::cout << "Enter stock ticker: ";
            std::cin >> ticker;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            ticker = Utils::toUpperCase(ticker);

            if (choice == 1) {
                if (!market.hasStock(ticker)) {
                    std::cout << "\n[ERROR] Ticker '" << ticker << "' not found in market.\n";
                } else {
                    user.addToWatchlist(ticker);
                    auth.saveUsersToFile(DATA_USERS_FILE);
                    std::cout << "\n[OK] " << ticker << " added to watchlist.\n";
                }
            } else if (choice == 2) {
                user.removeFromWatchlist(ticker);
                auth.saveUsersToFile(DATA_USERS_FILE);
                std::cout << "\n[OK] " << ticker << " removed from watchlist.\n";
            }
        }
    }

    // =========================================================================
    // ANALYTICS
    // =========================================================================

    void Menu::handleAnalytics() const {
        displayAnalyticsResults();
        displayUserAnalytics();
    }

    void Menu::displayAnalyticsResults() const {
        Utils::printHeader("Market Analytics");

        auto printList = [&](const std::string& title, const std::vector<Stock>& stocks) {
            std::cout << "\n  " << title << "\n";
            std::cout << "  " << std::string(50, '-') << "\n";
            if (stocks.empty()) {
                std::cout << "  No data available.\n";
                return;
            }
            for (size_t i = 0; i < stocks.size(); ++i) {
                std::cout << "  " << (i + 1) << ". "
                          << std::left << std::setw(8) << stocks[i].getTicker()
                          << std::setw(20) << stocks[i].getCompanyName()
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
        auto [topROIUser, topROI]     = auth.getHighestROIUser(market);

        std::cout << "  Highest Portfolio Value:\n";
        std::cout << "    " << topValueUser << "  ->  " << Utils::formatCurrency(topValue) << "\n";

        std::cout << "\n  Highest ROI:\n";
        std::cout << "    " << topROIUser << "  ->  " << Utils::formatPercentage(topROI) << "\n";
    }

    // =========================================================================
    // PRICE REFRESH
    // =========================================================================

    void Menu::handleRefreshPrices() {
        priceEngine.refreshPrices(market);
        FileManager::saveMarketData(market, DATA_STOCKS_FILE);
        std::cout << "\n[OK] Market prices refreshed. Analytics updated.\n";
        market.displayMarket();
    }

    // =========================================================================
    // ADMIN OPERATIONS
    // =========================================================================

    void Menu::handleAdminProfile() const {
        auth.getCurrentUser()->displayProfile();
    }

    void Menu::handleAddStock() {
        Utils::printHeader("Add New Stock");
        std::string ticker, name;

        std::cout << "Enter stock ticker (e.g. AAPL): ";
        std::cin >> ticker;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        ticker = Utils::toUpperCase(ticker);

        std::cout << "Enter company name: ";
        std::getline(std::cin, name);

        double price = getValidInput<double>("Enter initial price ($): ", 0.01, 1e7);

        try {
            Stock newStock(ticker, name, price);
            market.addStock(newStock);
            FileManager::saveMarketData(market, DATA_STOCKS_FILE);
            std::cout << "\n[OK] Stock '" << ticker << "' listed on market at " << Utils::formatCurrency(price) << ".\n";
        } catch (const MarketSystemException& e) {
            std::cout << "\n[ERROR] " << e.what() << "\n";
        }
    }

    void Menu::handleRemoveStock() {
        Utils::printHeader("Remove Stock");
        std::string ticker;

        std::cout << "Enter stock ticker to remove: ";
        std::cin >> ticker;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        ticker = Utils::toUpperCase(ticker);

        try {
            market.removeStock(ticker, auth.getUsers());
            FileManager::saveMarketData(market, DATA_STOCKS_FILE);
            std::cout << "\n[OK] Stock '" << ticker << "' removed from market.\n";
        } catch (const MarketSystemException& e) {
            std::cout << "\n[ERROR] " << e.what() << "\n";
        }
    }

    void Menu::handleUpdateStockPrice() {
        Utils::printHeader("Update Stock Price");
        std::string ticker;

        std::cout << "Enter stock ticker: ";
        std::cin >> ticker;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        ticker = Utils::toUpperCase(ticker);

        try {
            const Stock& stock = market.getStock(ticker);
            std::cout << "  Current price: " << Utils::formatCurrency(stock.getCurrentPrice()) << "\n";
            double newPrice = getValidInput<double>("Enter new price ($): ", 0.01, 1e7);

            market.updateStockPrice(ticker, newPrice);
            FileManager::saveMarketData(market, DATA_STOCKS_FILE);
            std::cout << "\n[OK] Price of '" << ticker << "' updated to " << Utils::formatCurrency(newPrice) << ".\n";
        } catch (const MarketSystemException& e) {
            std::cout << "\n[ERROR] " << e.what() << "\n";
        }
    }

    void Menu::handleAdminAnalytics() const {
        displayAnalyticsResults();
        displayUserAnalytics();
    }

    void Menu::handleMarketStatusToggle() {
        bool newStatus = !market.isMarketOpen();
        market.setMarketOpen(newStatus);
        std::cout << "\n[OK] Market is now " << (newStatus ? "OPEN" : "CLOSED") << ".\n";
    }

    void Menu::handleViewProfile() const {
        Utils::printHeader("Profile");
        auth.getCurrentUser()->displayProfile();
    }

} // namespace StockMarket
