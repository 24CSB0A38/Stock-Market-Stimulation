#include "services/Menu.h"
#include "services/Market.h"
#include "services/Authentication.h"
#include "services/PriceEngine.h"
#include "models/Transaction.h"
#include <vector>

int main() {
    // 1. Create all service objects (owns lifetime of all resources)
    StockMarket::Market         market;
    StockMarket::Authentication auth;
    StockMarket::PriceEngine    priceEngine(-5.0, 5.0); // price swings of -5% to +5%
    std::vector<StockMarket::Transaction> transactionLog;

    // 2. Pre-seed the default admin account (so system is always accessible)
    auth.registerAdmin("admin", "admin123");

    // 3. Create the Menu controller and inject all dependencies by reference
    StockMarket::Menu menu(market, auth, priceEngine, transactionLog);

    // 4. Hand full control to the menu loop (blocks until user chooses Exit)
    menu.run();

    return 0;
}
