/*
 * ============================================================================
 *  main.cpp — Entry Point for Stock Market Simulation
 * ============================================================================
 */

#include "Services.h"
#include <vector>

int main() {
    // 1. Create all service objects (owns lifetime of all resources)
    Market         market;
    Authentication auth;
    PriceEngine    priceEngine(-5.0, 5.0); // price swings of -5% to +5%
    std::vector<Transaction> transactionLog;

    // 2. Pre-seed the default admin account (so system is always accessible)
    auth.registerAdmin("admin", "admin123");

    // 3. Create the Menu controller and inject all dependencies by reference
    Menu menu(market, auth, priceEngine, transactionLog);

    // 4. Hand full control to the menu loop (blocks until user chooses Exit)
    menu.run();

    return 0;
}
