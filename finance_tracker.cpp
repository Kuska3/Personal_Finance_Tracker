// #include <bits/stdc++.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <memory>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <ctime>
#include <string>
using namespace std;


// ---------- ANSI Colors for CLI ----------
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"

// ---------- Utility functions ----------
static inline string trim(const string &s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

// Validate DD-MM-YYYY format
bool valid_date_format(const string &date) {
    if (date.size() != 10) return false;
    if (!isdigit(date[0]) || !isdigit(date[1])) return false; // DD
    if (date[2] != '-') return false;
    if (!isdigit(date[3]) || !isdigit(date[4])) return false; // MM
    if (date[5] != '-') return false;
    if (!isdigit(date[6]) || !isdigit(date[7]) || !isdigit(date[8]) || !isdigit(date[9])) return false; // YYYY
    return true;
}

// Extract "MM-YYYY" for monthly grouping
string month_key(const string &date) {
    if (!valid_date_format(date)) return "00-0000";
    return date.substr(3, 7); // Extract MM-YYYY
}

// Get current system date as DD-MM-YYYY
string current_date() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    int day = ltm->tm_mday;
    int month = 1 + ltm->tm_mon;
    int year = 1900 + ltm->tm_year;
    char buffer[11];
    sprintf(buffer, "%02d-%02d-%04d", day, month, year);
    return string(buffer);
}

// Forward declaration
class Account;

// ---------- Base Class ----------
class Transaction {
protected:
    double amount;
    string category;
    string date;
    string description;

public:
    Transaction(double amt, string cat, string dt, string desc = "")
        : amount(amt), category(move(cat)), date(move(dt)), description(move(desc)) {}

    virtual ~Transaction() = default;

    virtual void apply(Account &acc) const = 0;
    virtual string type() const = 0;

    double getAmount() const { return amount; }
    string getCategory() const { return category; }
    string getDate() const { return date; }
    string getDesc() const { return description; }
};

// ---------- Account Class ----------
class Account {
private:
    double balance;
    vector<shared_ptr<Transaction>> transactions;
    string filename = "finance_data.txt";

public:
    Account() : balance(0.0) {}

    void deposit(double amt) { balance += amt; }
    void withdraw(double amt) { balance -= amt; }

    double getBalance() const { return balance; }

    bool addTransaction(shared_ptr<Transaction> tx);

    const vector<shared_ptr<Transaction>>& getTransactions() const { return transactions; }

    vector<shared_ptr<Transaction>> getTransactionsForMonth(const string &monthYear) const {
        vector<shared_ptr<Transaction>> out;
        for (auto &t : transactions)
            if (month_key(t->getDate()) == monthYear)
                out.push_back(t);
        return out;
    }

    void saveToFile() const;
    void loadFromFile();
};

// ---------- Derived Classes ----------
class Income : public Transaction {
public:
    Income(double amt, string cat, string dt, string desc = "")
        : Transaction(amt, move(cat), move(dt), move(desc)) {}
    void apply(Account &acc) const override;
    string type() const override { return "Income"; }
};

class Expense : public Transaction {
public:
    Expense(double amt, string cat, string dt, string desc = "")
        : Transaction(amt, move(cat), move(dt), move(desc)) {}
    void apply(Account &acc) const override;
    string type() const override { return "Expense"; }
};

// ---------- Account method definitions ----------
bool Account::addTransaction(shared_ptr<Transaction> tx) {
    if (!tx) return false;
    if (tx->getAmount() <= 0.0) {
        cout << RED << "Error: amount must be positive.\n" << RESET;
        return false;
    }
    if (!valid_date_format(tx->getDate())) {
        cout << RED << "Error: invalid date format.\n" << RESET;
        return false;
    }
    tx->apply(*this);
    transactions.push_back(move(tx));
    return true;
}

void Account::saveToFile() const {
    ofstream file(filename);
    if (!file) {
        cerr << "Error saving data.\n";
        return;
    }
    file << fixed << setprecision(2);
    for (auto &t : transactions) {
        file << t->type() << "," << t->getAmount() << ","
             << t->getCategory() << "," << t->getDate() << ","
             << t->getDesc() << "\n";
    }
}

void Account::loadFromFile() {
    ifstream file(filename);
    if (!file) return;

    string line;
    int count = 0;
    while (getline(file, line)) {
        stringstream ss(line);
        string type, cat, date, desc, amtStr;
        double amt;
        getline(ss, type, ',');
        getline(ss, amtStr, ',');
        getline(ss, cat, ',');
        getline(ss, date, ',');
        getline(ss, desc, ',');
        amt = stod(amtStr);

        shared_ptr<Transaction> tx;
        if (type == "Income")
            tx = make_shared<Income>(amt, cat, date, desc);
        else if (type == "Expense")
            tx = make_shared<Expense>(amt, cat, date, desc);
        else
            continue;

        tx->apply(*this);
        transactions.push_back(tx);
        count++;
    }
    if (count > 0)
        cout << YELLOW << "Loaded " << count << " previous transactions.\n" << RESET;
}

// ---------- Income/Expense apply ----------
void Income::apply(Account &acc) const { acc.deposit(amount); }
void Expense::apply(Account &acc) const { acc.withdraw(amount); }

// ---------- ReportGenerator ----------
class ReportGenerator {
public:
    void printMonthlyReport(const Account &acc, const string &monthYear) const {
        double inc = 0, exp = 0;
        unordered_map<string, double> incCat, expCat;
        for (auto &t : acc.getTransactionsForMonth(monthYear)) {
            if (t->type() == "Income") {
                inc += t->getAmount();
                incCat[t->getCategory()] += t->getAmount();
            } else {
                exp += t->getAmount();
                expCat[t->getCategory()] += t->getAmount();
            }
        }
        cout << CYAN << "\n===== Monthly Report (" << monthYear << ") =====\n" << RESET;
        cout << GREEN << "Total Income : Rs " << inc << RESET << "\n";
        cout << RED   << "Total Expense: Rs " << exp << RESET << "\n";
        cout << YELLOW<< "Net Savings  : Rs " << (inc - exp) << RESET << "\n";
        cout << "\n-- Income by Category --\n";
        for (auto &p : incCat) cout << "  " << p.first << ": Rs " << p.second << "\n";
        cout << "\n-- Expense by Category --\n";
        for (auto &p : expCat) cout << "  " << p.first << ": Rs " << p.second << "\n";
    }

    void printAllTimeReport(const Account &acc) const {
        double inc = 0, exp = 0;
        for (auto &t : acc.getTransactions()) {
            if (t->type() == "Income") inc += t->getAmount();
            else exp += t->getAmount();
        }
        cout << CYAN << "\n===== All-Time Summary =====\n" << RESET;
        cout << GREEN << "Total Income : Rs " << inc << RESET << "\n";
        cout << RED   << "Total Expense: Rs " << exp << RESET << "\n";
        cout << YELLOW<< "Net Savings  : Rs " << (inc - exp) << RESET << "\n";
    }
};

// ---------- CLI ----------
void showMenu(double balance) {
    cout << CYAN << "\n--------------------------------\n";
    cout << "   Personal Finance Tracker     \n";
    cout << "--------------------------------\n" << RESET;
    cout << YELLOW << fixed << setprecision(2);
    cout << "Current Balance: Rs " << balance << "\n" << RESET;
    cout << "--------------------------------\n";
    cout << "1. Add Income\n";
    cout << "2. Add Expense\n";
    cout << "3. View Monthly Report\n";
    cout << "4. View All-time Summary\n";
    cout << "5. Exit\n";
    cout << "Choose an option: " << flush;
}

double readAmount() {
    while (true) {
        string input;
        getline(cin, input);
        try {
            double amt = stod(trim(input));
            if (amt > 0) return amt;
        } catch (...) {}
        cout << RED << "Enter a valid positive amount: " << RESET << flush;
    }
}

int main() {
    Account acc;
    ReportGenerator rg;
    acc.loadFromFile();

    while (true) {
        showMenu(acc.getBalance());
        string choice;
        getline(cin, choice);
        choice = trim(choice);

        if (choice == "1" || choice == "2") {
            bool isIncome = (choice == "1");
            cout << (isIncome ? GREEN "Adding Income...\n" RESET : RED "Adding Expense...\n" RESET);
            cout << "Amount: ";
            double amt = readAmount();
            cout << "Category: ";
            string cat; getline(cin, cat); cat = trim(cat);
            cout << "Date (DD-MM-YYYY, leave blank for today): ";
            string dt; getline(cin, dt); dt = trim(dt);
            if (dt.empty()) {
                dt = current_date();
                cout << YELLOW << "Auto-filled date: " << dt << "\n" << RESET;
            }
            if (!valid_date_format(dt)) {
                cout << RED << "Invalid date format! Use DD-MM-YYYY.\n" << RESET;
                continue;
            }
            cout << "Description (optional): ";
            string desc; getline(cin, desc); desc = trim(desc);

            shared_ptr<Transaction> tx;
            if (isIncome)
                tx = make_shared<Income>(amt, cat, dt, desc);
            else
                tx = make_shared<Expense>(amt, cat, dt, desc);

            if (acc.addTransaction(tx)) {
                cout << GREEN << "Transaction added successfully!\n" RESET;
                cout << YELLOW << "Current Balance: Rs " << fixed << setprecision(2) << acc.getBalance() << "\n" << RESET;
            }
        }
        else if (choice == "3") {
            cout << "Enter month and year (MM-YYYY): ";
            string my; getline(cin, my); my = trim(my);
            rg.printMonthlyReport(acc, my);
        }
        else if (choice == "4") {
            rg.printAllTimeReport(acc);
        }
        else if (choice == "5") {
            acc.saveToFile();
            cout << YELLOW << "All transactions saved successfully!\n" RESET;
            cout << CYAN << "Goodbye!\n" RESET;
            break;
        }
        else {
            cout << RED << "Invalid option. Try again.\n" RESET;
        }
    }

    return 0;
}
