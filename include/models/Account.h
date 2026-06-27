#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>

namespace StockMarket {

    /**
     * @brief Abstract base class representing an account in the system.
     * Demonstrates Abstraction, Encapsulation, and Polymorphism.
     */
    class Account {
    protected:
        std::string username;
        std::string password;
        std::string role; // "USER" or "ADMIN"

    public:
        Account(const std::string& username, const std::string& password, const std::string& role)
            : username(username), password(password), role(role) {}

        // Virtual destructor is critical for polymorphic base classes to prevent memory leaks
        virtual ~Account() = default;

        // Accessors (Const correct)
        std::string getUsername() const { return username; }
        std::string getPassword() const { return password; }
        std::string getRole() const { return role; }

        // Pure virtual function enforcing polymorphic implementation in derived classes
        virtual void displayProfile() const = 0;
    };

} // namespace StockMarket

#endif // ACCOUNT_H
