#include "services/Authentication.h"
#include "services/FileManager.h"
#include "utils/Exceptions.h"
#include "utils/Helper.h"
#include <iostream>

namespace StockMarket {

    Authentication::Authentication() : currentUser(nullptr) {}

    void Authentication::registerUser(const std::string& username, const std::string& password, double initialBalance) {
        std::string userLower = Utils::toUpperCase(username);
        if (users.find(userLower) != users.end() || admins.find(userLower) != admins.end()) {
            throw DuplicateUserException("Username '" + username + "' is already taken.");
        }

        if (initialBalance < 0.0) {
            throw InsufficientBalanceException("Initial deposit cannot be negative.");
        }

        users[userLower] = User(username, password, initialBalance);
    }

    void Authentication::registerAdmin(const std::string& username, const std::string& password) {
        std::string adminLower = Utils::toUpperCase(username);
        if (users.find(adminLower) != users.end() || admins.find(adminLower) != admins.end()) {
            throw DuplicateUserException("Username '" + username + "' is already taken.");
        }

        admins[adminLower] = Admin(username, password);
    }

    void Authentication::login(const std::string& username, const std::string& password) {
        std::string nameLower = Utils::toUpperCase(username);

        // Check if user account exists
        auto userIt = users.find(nameLower);
        if (userIt != users.end()) {
            if (userIt->second.getPassword() == password) {
                // currentUser points directly to the User object stored in users map
                currentUser = &userIt->second;
                return;
            }
            throw AuthenticationFailedException("Incorrect password for username '" + username + "'.");
        }

        // Check if admin account exists
        auto adminIt = admins.find(nameLower);
        if (adminIt != admins.end()) {
            if (adminIt->second.getPassword() == password) {
                // currentUser points directly to the Admin object stored in admins map
                currentUser = &adminIt->second;
                return;
            }
            throw AuthenticationFailedException("Incorrect password for admin username '" + username + "'.");
        }

        throw AuthenticationFailedException("Username '" + username + "' not found.");
    }

    void Authentication::logout() {
        currentUser = nullptr;
    }

    Account* Authentication::getCurrentUser() const {
        return currentUser;
    }

    bool Authentication::isLoggedIn() const {
        return currentUser != nullptr;
    }

    User& Authentication::getCurrentUserAsUser() {
        if (!currentUser || currentUser->getRole() != "USER") {
            throw AuthenticationFailedException("No active user session found.");
        }
        // Downcast safe: we verified that the active session is a USER role.
        // Direct cast without dynamic_cast since smart pointers/polymorphic casts are avoided where simple checks suffice.
        return *static_cast<User*>(currentUser);
    }

    const User& Authentication::getCurrentUserAsUser() const {
        if (!currentUser || currentUser->getRole() != "USER") {
            throw AuthenticationFailedException("No active user session found.");
        }
        return *static_cast<const User*>(currentUser);
    }

    std::unordered_map<std::string, User>& Authentication::getUsers() {
        return users;
    }

    const std::unordered_map<std::string, User>& Authentication::getUsers() const {
        return users;
    }

    std::unordered_map<std::string, Admin>& Authentication::getAdmins() {
        return admins;
    }

    const std::unordered_map<std::string, Admin>& Authentication::getAdmins() const {
        return admins;
    }

    void Authentication::loadUsersFromFile(const std::string& filepath) {
        FileManager::loadUserData(users, admins, filepath);
    }

    void Authentication::saveUsersToFile(const std::string& filepath) const {
        FileManager::saveUserData(users, admins, filepath);
    }

    std::pair<std::string, double> Authentication::getHighestPortfolioValueUser(const Market& market) const {
        std::string topUser = "None";
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

    std::pair<std::string, double> Authentication::getHighestROIUser(const Market& market) const {
        std::string topUser = "None";
        double maxROI = -999999.0;
        bool found = false;

        for (const auto& pair : users) {
            // Only calculate ROI for users who have active investments
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

} // namespace StockMarket
