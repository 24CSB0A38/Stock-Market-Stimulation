# Stock Market Simulation & Portfolio Management System

A fully functional, console-based **Stock Market Simulation** built in **C++17** using only the Standard Template Library. Designed to demonstrate advanced Object-Oriented Programming, clean software architecture, modular design, and real business logic — making it interview-ready for roles at companies like Microsoft, Google, Goldman Sachs, Adobe, and DE Shaw.

---

## Project Overview

This system simulates a real-world stock trading environment where users can register accounts, deposit funds, buy and sell stocks, track portfolio performance with live profit/loss calculations, maintain a personal watchlist, and view market analytics — all with persistent data storage using text files.

Administrators can manage the market: add or remove stock listings, manually update prices, toggle the market open/closed, and view cross-user analytics.

A built-in **Price Engine** simulates market volatility by applying random percentage-based price changes to all stocks, updating price history after every refresh cycle.

---

## Features

### User Module
- Register a new trading account with an initial deposit
- Login / Logout with credential validation
- View profile summary (balance, holdings count, watchlist size)

### Market Module
- View all listed stocks with current price and daily change
- Search a specific stock by ticker for detailed information
- View last 5 price history entries per stock

### Trading Module
- **Buy stocks** — validates wallet balance, updates portfolio with weighted average cost basis
- **Sell stocks** — validates share ownership, credits proceeds to wallet
- All trades blocked when market is **CLOSED**
- Every trade is logged to `transactions.txt` on disk

### Portfolio Module
- View all holdings: shares, average buy price, market price, investment, current value, and net P&L per stock
- Dynamic summary: Total Investment, Current Value, Net Profit/Loss, and ROI%
- Full transaction history display

### Watchlist Module
- Add stocks to personal watchlist (no duplicates — backed by `std::unordered_set`)
- Remove stocks from watchlist
- Live price preview for each watched stock

### Admin Module
- Add new stock to market (ticker, company, initial price)
- Remove stock (blocked if any user currently holds shares — enforced business rule)
- Manually update any stock's price
- Toggle market OPEN / CLOSED
- View market analytics and cross-user statistics

### Analytics Engine
- **Top Gainers** — stocks with highest % gain from previous price
- **Top Losers** — stocks with highest % loss from previous price
- **Most Traded** — ranked by total shares volume
- **Highest Volume ($)** — ranked by shares × current price
- **Highest Portfolio Value** — user with maximum current portfolio value
- **Highest ROI** — user with maximum return on investment percentage

### Price Engine
- Simulates random market price movements (-5% to +5% per refresh)
- Updates `currentPrice`, `previousPrice`, and `priceHistory` on every refresh
- Uses `std::mt19937` Mersenne Twister for statistically uniform distribution

---

## Technology Stack

| Category | Technology |
|----------|------------|
| Language | C++17 |
| Libraries | Standard Template Library only |
| Build Tool | g++ / clang++ |
| Platform | Cross-platform (Linux, macOS, Windows) |
| Persistence | Plain text files (CSV format) |
| UI | Console (terminal) |

---

## Key C++ Concepts Demonstrated

| Concept | Where Used |
|---------|-----------|
| Abstraction | `Account` abstract base class with pure virtual `displayProfile()` |
| Inheritance | `User` and `Admin` derive from `Account` |
| Polymorphism | `displayProfile()` resolved at runtime through base pointer |
| Encapsulation | All class members are private; exposed via const-correct accessors |
| Composition | `User` owns a `Portfolio` and an `unordered_set` watchlist |
| Templates | `getValidInput<T>()` handles safe numeric input for any type |
| Custom Exceptions | Six exception classes inheriting from `std::runtime_error` |
| `std::unordered_map` | $O(1)$ stock lookup by ticker, $O(1)$ user lookup by username |
| `std::map` | Alphabetically sorted portfolio holdings |
| `std::unordered_set` | Duplicate-free watchlist with $O(1)$ lookup |
| `std::priority_queue` | Efficient analytics extraction (top gainers, losers, volume) |
| `std::vector` | Price history per stock, transaction log |
| `std::algorithm` | `std::sort` for display ordering; `std::transform` for uppercase |
| File Streams | `std::ifstream` / `std::ofstream` for full persistence |
| Const Correctness | `const` on all read-only member functions |
| Static Members | `Transaction::deserialize()` as a static factory function |
| Namespaces | Entire project wrapped in `StockMarket` namespace |
| Modern C++17 | Structured bindings (`auto [a, b] = ...`), `std::mt19937` |

---

## Project Structure

```
StockMarketSimulation/
├── include/
│   ├── models/
│   │   ├── Account.h          # Abstract base class (pure virtual)
│   │   ├── Admin.h            # Admin account (inherits Account)
│   │   ├── Portfolio.h        # Holdings manager with dynamic analytics
│   │   ├── Stock.h            # Stock data model with price history
│   │   ├── Transaction.h      # Trade record with serialization
│   │   └── User.h             # User account with portfolio & watchlist
│   ├── services/
│   │   ├── Authentication.h   # Login/register & session manager
│   │   ├── FileManager.h      # Text file persistence (load/save)
│   │   ├── Market.h           # Stock registry with analytics engine
│   │   ├── Menu.h             # CLI router and UI controller
│   │   └── PriceEngine.h      # Random price simulation engine
│   └── utils/
│       ├── Exceptions.h       # Custom exception hierarchy
│       ├── Helper.h           # Timestamp, formatting, console utilities
│       └── Templates.h        # Generic validated input reader
├── src/
│   ├── models/
│   │   ├── Admin.cpp
│   │   ├── Portfolio.cpp
│   │   ├── Stock.cpp
│   │   ├── Transaction.cpp
│   │   └── User.cpp
│   ├── services/
│   │   ├── Authentication.cpp
│   │   ├── FileManager.cpp
│   │   ├── Market.cpp
│   │   ├── Menu.cpp
│   │   └── PriceEngine.cpp
│   ├── utils/
│   │   └── Helper.cpp
│   └── main.cpp
├── data/
│   ├── stocks.txt             # Market stock data (auto-created)
│   ├── users.txt              # User & admin accounts (auto-created)
│   └── transactions.txt       # Trade ledger (auto-created)
└── README.md
```

---

## How to Compile

### Prerequisites
- `g++` with C++17 support (`g++ --version` should show 7.0 or higher)

### Compile Command

```bash
g++ -std=c++17 -Iinclude \
    src/utils/Helper.cpp \
    src/models/Stock.cpp \
    src/models/Transaction.cpp \
    src/models/User.cpp \
    src/models/Admin.cpp \
    src/models/Portfolio.cpp \
    src/services/Market.cpp \
    src/services/PriceEngine.cpp \
    src/services/FileManager.cpp \
    src/services/Authentication.cpp \
    src/services/Menu.cpp \
    src/main.cpp \
    -o StockSim
```

### One-liner (macOS/Linux with globbing)

```bash
g++ -std=c++17 -Iinclude src/utils/*.cpp src/models/*.cpp src/services/*.cpp src/main.cpp -o StockSim
```

> **Note:** On some shells (like zsh on macOS), globbing with `**` may require `setopt globstar` or explicit path enumeration as shown above.

---

## How to Run

```bash
./StockSim
```

The program will automatically load stock and user data from the `data/` directory on startup and save all changes on logout or exit.

### Default Admin Credentials
| Field | Value |
|-------|-------|
| Username | `admin` |
| Password | `admin123` |

---

## Sample Session

```
╔══════════════════════════════════════════════════╗
║     Stock Market Simulation & Portfolio Mgmt.    ║
║              C++17  |  STL  |  OOP               ║
╚══════════════════════════════════════════════════╝

==================================================
  Main Menu
==================================================
  [1] Register
  [2] Login
  [0] Exit

Enter choice: 1

==================================================
  Register New Account
==================================================
Enter username: alice
Enter password: alice123
Enter initial deposit ($): 50000

[OK] Account created successfully. Please login.

Enter choice: 2
Enter username: alice
Enter password: alice123

[OK] Welcome back, alice!

==================================================
  User Dashboard  [Market: OPEN]
==================================================
  [3] Buy Stock
Enter choice: 3

Enter stock ticker to buy: AAPL
  Stock: Apple Inc. | Current Price: $178.50 | Your Balance: $50000.00
Enter number of shares to buy: 100
  Total Cost: $17850.00
  Confirm purchase? [1=Yes / 0=No]: 1

[OK] Bought 100 shares of AAPL at $178.50
  Remaining Balance: $32150.00

  [5] View Portfolio
Enter choice: 5

==================================================
  Portfolio  |  alice
==================================================
  Wallet Balance:    $32150.00

Ticker  Shares  Avg Buy Price  Market Price   Investment     Curr. Value   Net P&L
------------------------------------------------------------------------------------
AAPL    100     $178.50        $178.50        $17850.00      $17850.00     $0.00

  PORTFOLIO SUMMARY
  Total Investment:  $17850.00
  Current Value:     $17850.00
  Net Profit/Loss:   $0.00
  ROI:               +0.00%

  [9] Refresh Market Prices
Enter choice: 9

[OK] Market prices refreshed. Analytics updated.

Ticker  Company Name           Price        Change      Shares Traded
---------------------------------------------------------------------
AAPL    Apple Inc.             $181.23      +1.52%      100
AMZN    Amazon.com Inc.        $195.10      -1.61%      0
...

  [5] View Portfolio
Enter choice: 5

  PORTFOLIO SUMMARY
  Total Investment:  $17850.00
  Current Value:     $18123.00
  Net Profit/Loss:   $273.00
  ROI:               +1.53%
```

---

## Business Rules Enforced

| Rule | Implementation |
|------|---------------|
| No buy without sufficient balance | `User::debitBalance()` throws `InsufficientBalanceException` |
| No sell without owned shares | `Portfolio::sell()` throws `InvalidShareQuantityException` |
| No trades when market is CLOSED | `Menu` checks `market.isMarketOpen()` before every trade |
| No duplicate usernames | `Authentication::registerUser()` throws `DuplicateUserException` |
| Admin cannot remove stock with shareholders | `Market::removeStock()` scans all user portfolios |
| No duplicate watchlist entries | `std::unordered_set` enforces uniqueness natively |
| Stocks cannot have zero/negative prices | `Stock` constructor and `updatePrice()` validate |

---

## Data File Formats

### stocks.txt
```
TICKER,Company Name,currentPrice,previousPrice,initialPrice,sharesTraded,hist1;hist2;...
```
Example:
```
AAPL,Apple Inc.,181.23,178.50,178.50,100,178.50;181.23
```

### users.txt
```
username,password,ROLE,balance,watchlist(;sep),holdings(TICKER:shares:avgPrice;...)
```
Example:
```
alice,alice123,USER,32150.00,AAPL;MSFT,AAPL:100:178.50
```

### transactions.txt
```
TxnID,TICKER,BUY|SELL,quantity,price,YYYY-MM-DD HH:MM:SS
```
Example:
```
TXN1-2026-06-27,AAPL,BUY,100,178.50,2026-06-27 15:30:00
```

---

## Future Improvements

| Enhancement | Description |
|-------------|-------------|
| Order Types | Limit orders, stop-loss, and market orders |
| Portfolio Visualization | ASCII chart of portfolio value over time using price history |
| Multi-session Persistence | Store transaction owner alongside each trade for multi-user history filtering |
| Dividend Simulation | Periodic dividend credits based on shares held |
| News Event Engine | Random company events that cause larger price swings |
| Admin Audit Log | Track every admin action (price changes, stock additions) separately |
| Password Hashing | Replace plain-text password storage with a simple hash function |
| Unit Tests | Isolated test cases for Portfolio analytics, exception paths, and FileManager parsing |

---

## Interview Talking Points

- **"Walk me through your class hierarchy."**
  `Account` is a pure abstract base class. `User` and `Admin` derive from it and override `displayProfile()`. This demonstrates the four pillars of OOP: abstraction, encapsulation, inheritance, and polymorphism.

- **"Why did you use `unordered_map` for stocks but `map` for portfolio holdings?"**
  `unordered_map` gives $O(1)$ average lookup for the market — critical when users buy/sell by ticker. `map` in the portfolio gives sorted display output for free, which makes the portfolio view clean and consistent.

- **"How did you avoid memory leaks without smart pointers?"**
  `Authentication` stores `User` and `Admin` objects by value in two separate `unordered_map`s. The `currentUser` pointer is non-owning — it points into an element already owned by those maps. When the maps go out of scope in `main()`, all objects are destroyed automatically. No `new` or `delete` anywhere.

- **"How does your analytics engine work?"**
  Four `std::priority_queue`s with custom comparator functors extract Top Gainers, Losers, Volume, and Trading Activity in $O(N \log K)$ time — more efficient than sorting all stocks when only the top $K$ are needed.
