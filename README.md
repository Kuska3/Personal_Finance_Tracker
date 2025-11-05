# Personal Finance Tracker (C++)

A simple, menu-driven Personal Finance Tracker built in C++. This command-line application allows you to log your income and expenses, view summary reports, and persists all data to a local file.

This project is built with modern C++ (C++11 or newer) and demonstrates object-oriented principles, smart pointers, and STL containers.

## 📊 Features

* **Add Transactions:** Easily add income or expense entries with an amount, category, date, and optional description.
* **Persistent Storage:** All transactions are automatically saved to `finance_data.txt` on exit and reloaded on startup.
* **Monthly Reports:** View a detailed breakdown of income vs. expenses for any given month (e.g., "11-2025"), including-category summaries.
* **All-Time Summary:** Get a quick overview of your total lifetime income, total expenses, and net savings.
* **Smart Date Entry:** Automatically uses the current system date if you leave the date field blank.
* **Input Validation:** Ensures valid positive amounts and correct "DD-MM-YYYY" date formats.
* **Color-Coded Interface:** Uses ANSI color codes for a more readable and engaging command-line experience.

## 📸 Demo (Example Usage)

Here is what the main menu and a monthly report look like in the terminal:

```text
--------------------------------
   Personal Finance Tracker     
--------------------------------
Current Balance: Rs 15000.00
--------------------------------
1. Add Income
2. Add Expense
3. View Monthly Report
4. View All-time Summary
5. Exit
Choose an option: 3

Enter month and year (MM-YYYY): 11-2025

===== Monthly Report (11-2025) =====
Total Income : Rs 50000.00
Total Expense: Rs 35000.00
Net Savings  : Rs 15000.00

-- Income by Category --
  Salary: Rs 50000.00

-- Expense by Category --
  Rent: Rs 20000.00
  Groceries: Rs 10000.00
  Utilities: Rs 5000.00
