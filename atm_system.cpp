#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <stdexcept>
#include <limits>
#include <ctime>

using namespace std;

// Transaction structure to store transaction details
struct Transaction {
    string type;
    double amount;
    double balanceAfter;
    string timestamp;
    string details;
    
    Transaction(string t, double amt, double bal, string det = "") 
        : type(t), amount(amt), balanceAfter(bal), details(det) {
        time_t now = time(0);
        timestamp = ctime(&now);
        timestamp.pop_back(); // Remove newline
    }
};

// Custom exception classes
class InsufficientFundsException : public runtime_error {
public:
    InsufficientFundsException() : runtime_error("Insufficient funds in account") {}
};

class InvalidAmountException : public runtime_error {
public:
    InvalidAmountException() : runtime_error("Invalid amount entered") {}
};

class AuthenticationException : public runtime_error {
public:
    AuthenticationException() : runtime_error("Authentication failed") {}
};

class AccountNotFoundException : public runtime_error {
public:
    AccountNotFoundException() : runtime_error("Recipient account not found") {}
};

class SameAccountException : public runtime_error {
public:
    SameAccountException() : runtime_error("Cannot transfer to the same account") {}
};

// Account class
class Account {
private:
    string accountNumber;
    string pin;
    string accountHolder;
    double balance;
    vector<Transaction> transactionHistory;
    
public:
    Account(string accNum, string p, string holder, double initialBalance = 0.0) 
        : accountNumber(accNum), pin(p), accountHolder(holder), balance(initialBalance) {}
    
    // Getters
    string getAccountNumber() const { return accountNumber; }
    string getAccountHolder() const { return accountHolder; }
    double getBalance() const { return balance; }
    
    // Verify PIN
    bool verifyPin(const string& inputPin) const {
        return pin == inputPin;
    }
    
    // Deposit money
    void deposit(double amount, string details = "") {
        if (amount <= 0) {
            throw InvalidAmountException();
        }
        balance += amount;
        transactionHistory.push_back(Transaction("Deposit", amount, balance, details));
    }
    
    // Withdraw money
    void withdraw(double amount, string details = "") {
        if (amount <= 0) {
            throw InvalidAmountException();
        }
        if (amount > balance) {
            throw InsufficientFundsException();
        }
        balance -= amount;
        transactionHistory.push_back(Transaction("Withdrawal", amount, balance, details));
    }
    
    // Display transaction history
    void displayTransactionHistory() const {
        if (transactionHistory.empty()) {
            cout << "\n=== No transactions found ===\n";
            return;
        }
        
        cout << "\n========== TRANSACTION HISTORY ==========\n";
        cout << left << setw(15) << "Type" 
             << setw(15) << "Amount" 
             << setw(15) << "Balance" 
             << "Details\n";
        cout << string(70, '-') << endl;
        
        for (const auto& trans : transactionHistory) {
            cout << left << setw(15) << trans.type
                 << "$" << setw(14) << fixed << setprecision(2) << trans.amount
                 << "$" << setw(14) << trans.balanceAfter;
            if (!trans.details.empty()) {
                cout << trans.details;
            }
            cout << "\n" << string(45, ' ') << trans.timestamp << endl;
        }
        cout << "=========================================\n";
    }
};

// ATM class
class ATM {
private:
    vector<Account> accounts;
    Account* currentAccount;
    
    void clearInputBuffer() {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    
    Account* findAccount(const string& accNum) {
        for (auto& acc : accounts) {
            if (acc.getAccountNumber() == accNum) {
                return &acc;
            }
        }
        return nullptr;
    }
    
public:
    ATM() : currentAccount(nullptr) {
        // Pre-load some accounts for testing
        accounts.push_back(Account("1001", "1234", "Ehindero Henry", 5000000.0));
        accounts.push_back(Account("1002", "5678", "Juria Momoh", 3000.0));
        accounts.push_back(Account("1003", "9999", "Stephen", 10000.0));
        accounts.push_back(Account("1004", "3829", "Ajao Michael", 100.0));
        accounts.push_back(Account("1005", "4783", "Deji", 10000.0));
        accounts.push_back(Account("1006", "2378", "Omotola", 0.0));
    }
    
    // User authentication
    bool authenticate() {
        string accNum, pin;
        
        cout << "\n========== ATM LOGIN ==========\n";
        cout << "Enter Account Number: ";
        cin >> accNum;
        cout << "Enter PIN: ";
        cin >> pin;
        
        try {
            Account* acc = findAccount(accNum);
            if (acc == nullptr || !acc->verifyPin(pin)) {
                throw AuthenticationException();
            }
            currentAccount = acc;
            cout << "\nLogin successful! Welcome, " << currentAccount->getAccountHolder() << "!\n";
            return true;
        } catch (const AuthenticationException& e) {
            cout << "\nError: " << e.what() << endl;
            cout << "Please try again.\n";
            return false;
        }
    }
    
    // Check balance
    void checkBalance() {
        if (currentAccount == nullptr) return;
        
        cout << "\n========== BALANCE INQUIRY ==========\n";
        cout << "Account Holder: " << currentAccount->getAccountHolder() << endl;
        cout << "Account Number: " << currentAccount->getAccountNumber() << endl;
        cout << "Current Balance: $" << fixed << setprecision(2) 
             << currentAccount->getBalance() << endl;
        cout << "=====================================\n";
    }
    
    // Deposit money
    void deposit() {
        if (currentAccount == nullptr) return;
        
        double amount;
        cout << "\n========== DEPOSIT ==========\n";
        cout << "Enter deposit amount: $";
        
        if (!(cin >> amount)) {
            clearInputBuffer();
            cout << "Error: Invalid input. Please enter a valid number.\n";
            return;
        }
        
        try {
            currentAccount->deposit(amount);
            cout << "\nDeposit successful!\n";
            cout << "New Balance: $" << fixed << setprecision(2) 
                 << currentAccount->getBalance() << endl;
        } catch (const InvalidAmountException& e) {
            cout << "\nError: " << e.what() << endl;
        }
    }
    
    // Withdraw money
    void withdraw() {
        if (currentAccount == nullptr) return;
        
        double amount;
        cout << "\n========== WITHDRAWAL ==========\n";
        cout << "Current Balance: $" << fixed << setprecision(2) 
             << currentAccount->getBalance() << endl;
        cout << "Enter withdrawal amount: $";
        
        if (!(cin >> amount)) {
            clearInputBuffer();
            cout << "Error: Invalid input. Please enter a valid number.\n";
            return;
        }
        
        try {
            currentAccount->withdraw(amount);
            cout << "\nWithdrawal successful!\n";
            cout << "New Balance: $" << fixed << setprecision(2) 
                 << currentAccount->getBalance() << endl;
        } catch (const InsufficientFundsException& e) {
            cout << "\nError: " << e.what() << endl;
        } catch (const InvalidAmountException& e) {
            cout << "\nError: " << e.what() << endl;
        }
    }
    
    // Transfer money to another account1006
    
    void transfer() {
        if (currentAccount == nullptr) return;
        
        string recipientAccNum;
        double amount;
        
        cout << "\n========== TRANSFER MONEY ==========\n";
        cout << "Current Balance: $" << fixed << setprecision(2) 
             << currentAccount->getBalance() << endl;
        cout << "Enter recipient account number: ";
        cin >> recipientAccNum;
        
        try {
            // Check if recipient account exists
            Account* recipientAccount = findAccount(recipientAccNum);
            if (recipientAccount == nullptr) {
                throw AccountNotFoundException();
            }
            
            // Check if trying to transfer to same account
            if (recipientAccount->getAccountNumber() == currentAccount->getAccountNumber()) {
                throw SameAccountException();
            }
            
            cout << "Recipient: " << recipientAccount->getAccountHolder() << endl;
            cout << "Enter transfer amount: $";
            
            if (!(cin >> amount)) {
                clearInputBuffer();
                cout << "Error: Invalid input. Please enter a valid number.\n";
                return;
            }
            
            // Perform the transfer
            string senderDetails = "Transfer to " + recipientAccount->getAccountHolder() + 
                                 " (Acc: " + recipientAccount->getAccountNumber() + ")";
            string recipientDetails = "Transfer from " + currentAccount->getAccountHolder() + 
                                    " (Acc: " + currentAccount->getAccountNumber() + ")";
            
            currentAccount->withdraw(amount, senderDetails);
            recipientAccount->deposit(amount, recipientDetails);
            
            cout << "\n========== TRANSFER SUCCESSFUL ==========\n";
            cout << "Transferred: $" << fixed << setprecision(2) << amount << endl;
            cout << "To: " << recipientAccount->getAccountHolder() << endl;
            cout << "Your New Balance: $" << currentAccount->getBalance() << endl;
            cout << "=========================================\n";
            
        } catch (const AccountNotFoundException& e) {
            cout << "\nError: " << e.what() << endl;
        } catch (const SameAccountException& e) {
            cout << "\nError: " << e.what() << endl;
        } catch (const InsufficientFundsException& e) {
            cout << "\nError: " << e.what() << endl;
        } catch (const InvalidAmountException& e) {
            cout << "\nError: " << e.what() << endl;
        }
    }
    
    // View transaction history
    void viewTransactionHistory() {
        if (currentAccount == nullptr) return;
        currentAccount->displayTransactionHistory();
    }
    
    // Main menu
    void showMenu() {
        int choice;
        
        do {
            cout << "\n========== ATM MAIN MENU ==========\n";
            cout << "1. Balance Inquiry\n";
            cout << "2. Deposit\n";
            cout << "3. Withdrawal\n";
            cout << "4. Transfer Money\n";
            cout << "5. Transaction History\n";
            cout << "6. Logout\n";
            cout << "===================================\n";
            cout << "Enter your choice: ";
            
            if (!(cin >> choice)) {
                clearInputBuffer();
                cout << "Invalid input! Please enter a number.\n";
                continue;
            }
            
            switch (choice) {
                case 1:
                    checkBalance();
                    break;
                case 2:
                    deposit();
                    break;
                case 3:
                    withdraw();
                    break;
                case 4:
                    transfer();
                    break;
                case 5:
                    viewTransactionHistory();
                    break;
                case 6:
                    cout << "\nThank you for using our ATM. Goodbye!\n";
                    currentAccount = nullptr;
                    break;
                default:
                    cout << "\nInvalid choice! Please try again.\n";
            }
        } while (choice != 6);
    }
    
    // Display test accounts
    void displayTestAccounts() {
        cout << "\n========== TEST ACCOUNTS ==========\n";
        cout << "Account: 1001, PIN: 1234, Balance: $5000\n";
        cout << "Account: 1002, PIN: 5678, Balance: $3000\n";
        cout << "Account: 1003, PIN: 9999, Balance: $10000\n";
        cout << "===================================\n";
    }
};

int main() {
    ATM atm;
    
    cout << "========================================\n";
    cout << "   WELCOME TO ATM SIMULATION SYSTEM\n";
    cout << "========================================\n";
    
    atm.displayTestAccounts();
    
    while (true) {
        if (atm.authenticate()) {
            atm.showMenu();
        }
        
        char choice;
        cout << "\nDo you want to login with another account? (y/n): ";
        cin >> choice;
        if (choice != 'y' && choice != 'Y') {
            cout << "\nThank you for using our ATM system!\n";
            break;
        }
    }
    
    return 0;
}