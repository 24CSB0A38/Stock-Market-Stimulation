#ifndef AUTHENTICATION_H
#define AUTHENTICATION_H

#include "models/User.h"
#include "models/Admin.h"
#include <unordered_map>
#include <string>

namespace StockMarket {

    /**
     * @brief Manages user and admin sessions, registration, and logins.
     * Demonstrates safe session tracking, clean reference handling, and map structures.
     */
    class Authentication {
    private:
        std::unordered_map<std::string, User> users;
        std::unordered_map<std::string, Admin> admins;
        
        // Non-owning pointer referencing either a User in the users map or an Admin in the admins map.
        // Avoids smart pointers and manual memory management (deletes) completely.
        Account* currentUser;

    public:
        Authentication();

        // Session controls
        void registerUser(const std::string& username, const std::string& password, double initialBalance = 10000.0);
        void registerAdmin(const std::string& username, const std::string& password);
        void login(const std::string& username, const std::string& password);
        void logout();

        // Getters
        Account* getCurrentUser() const;
        bool isLoggedIn() const;
        
        // Helper to retrieve the current active user as a writable reference
        User& getCurrentUserAsUser();
        const User& getCurrentUserAsUser() const;

        // Directory registries
        std::unordered_map<std::string, User>& getUsers();
        const std::unordered_map<std::string, User>& getUsers() const;
        std::unordered_map<std::string, Admin>& getAdmins();
        const std::unordered_map<std::string, Admin>& getAdmins() const;

        // Persistence wrappers
        void loadUsersFromFile(const std::string& filepath);
        void saveUsersToFile(const std::string& filepath) const;

        // Dynamic cross-user statistics (Calculated dynamically)
        std::pair<std::string, double> getHighestPortfolioValueUser(const Market& market) const;
        std::pair<std::string, double> getHighestROIUser(const Market& market) const;
    };

} // namespace StockMarket

#endif // AUTHENTICATION_H
