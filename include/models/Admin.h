#ifndef ADMIN_H
#define ADMIN_H

#include "models/Account.h"
#include <string>

namespace StockMarket {

    /**
     * @brief Class representing an administrator.
     * Demonstrates Inheritance and Polymorphism.
     */
    class Admin : public Account {
    public:
        // Default constructor
        Admin();

        // Parameterized constructor
        Admin(const std::string& username, const std::string& password);

        // Polymorphic implementation of displayProfile
        void displayProfile() const override;
    };

} // namespace StockMarket

#endif // ADMIN_H
