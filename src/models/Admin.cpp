#include "models/Admin.h"
#include <iostream>

namespace StockMarket {

    Admin::Admin() 
        : Account("", "", "ADMIN") {}

    Admin::Admin(const std::string& username, const std::string& password)
        : Account(username, password, "ADMIN") {}

    void Admin::displayProfile() const {
        std::cout << "--- Admin Profile ---\n";
        std::cout << "Username: " << username << "\n";
        std::cout << "Role:     " << role << " (Full System Privileges)\n";
    }

} // namespace StockMarket
