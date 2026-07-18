#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <sstream>
#include <fstream>

using namespace std;

// Function to generate unique transaction ID using timestamp
string generateTransactionID() {
    time_t now = time(0);
    tm* ltm = localtime(&now);

    stringstream ss;
    ss << "TXN" << (1900 + ltm->tm_year) << (1 + ltm->tm_mon) << ltm->tm_mday 
       << ltm->tm_hour << ltm->tm_min << ltm->tm_sec;
    return ss.str();
}

// Base class: User
class User {
protected:
    string name;
    string email;
    string pin;
    string pinFile = "pin.txt";

    string readSavedPin() {
        ifstream file(pinFile);
        string savedPin;
        if (file) {
            file >> savedPin;
            file.close();
        } else {
            savedPin = "953469";
        }
        return savedPin;
    }

    void savePinToFile() {
        ofstream file(pinFile);
        if (file) {
            file << pin;
            file.close();
        }
    }

public:
    User(string name, string email) : name(name), email(email) {
        pin = readSavedPin();
        cout << "[User Constructor] Initialized user: " << name << endl;
    }

    virtual ~User() {
        cout << "[User Destructor] Cleaning up user: " << name << endl;
    }

    bool authenticate() {
        string inputPin;
        cout << "Enter PIN to access wallet: ";
        cin >> inputPin;
        return inputPin == pin;
    }

    void changePin() {
        string currentPin;
        cout << "Enter current PIN: ";
        cin >> currentPin;

        if (currentPin == pin) {
            string newPin;
            cout << "Enter new PIN: ";
            cin >> newPin;
            pin = newPin;
            savePinToFile();
            cout << "PIN successfully changed and saved!\n";
        } else {
            cout << "Incorrect current PIN! PIN change failed.\n";
        }
    }

    virtual void displayInfo() const {
        cout << "User: " << name << ", Email: " << email << endl;
    }
};

// Transaction Class
class Transaction {
private:
    string transactionId;
    double amount;
    string type;
    string category;

public:
    Transaction(string id, double amt, string t, string cat) 
        : transactionId(id), amount(amt), type(t), category(cat) {
        cout << "[Transaction Constructor] Created: " << transactionId << endl;
    }

    ~Transaction() {
        cout << "[Transaction Destructor] Deleted: " << transactionId << endl;
    }

    void displayTransaction() const {
        cout << "Transaction ID: " << transactionId << ", Amount: $" << amount 
             << ", Type: " << type << ", Category: " << category << endl;
    }

    string getType() const { return type; }
};

// Transaction History Class
class TransactionHistory {
private:
    vector<Transaction> transactions;

public:
    TransactionHistory() {
        cout << "[TransactionHistory Constructor] Initialized.\n";
    }

    ~TransactionHistory() {
        cout << "[TransactionHistory Destructor] Cleaning up transactions.\n";
    }

    void addTransaction(const string& id, double amount, const string& type, const string& category) {
        transactions.push_back(Transaction(id, amount, type, category));
    }

    void displayHistory() const {
        cout << "\n===== Transaction History =====" << endl;
        if (transactions.empty()) {
            cout << "No transactions yet!" << endl;
        } else {
            for (const auto& transaction : transactions) {
                transaction.displayTransaction();
            }
        }
    }

    void displayFilteredHistory(const string& type) const {
        cout << "\n===== " << (type == "credit" ? "Credits" : "Debits") << " Only =====" << endl;
        bool found = false;
        for (const auto& transaction : transactions) {
            if (transaction.getType() == type) {
                transaction.displayTransaction();
                found = true;
            }
        }
        if (!found) {
            cout << "No " << type << " transactions found.\n";
        }
    }
};

// Wallet Class
class Wallet : public User {
private:
    double balance;
    TransactionHistory history;
    int rewardPoints;

public:
    Wallet(string name, string email) 
        : User(name, email), balance(0.0), rewardPoints(0) {
        cout << "[Wallet Constructor] Wallet created for user.\n";
    }

    ~Wallet() {
        cout << "[Wallet Destructor] Cleaning up wallet.\n";
    }

    void addFunds(double amount) {
        if (amount > 0) {
            balance += amount;
            history.addTransaction(generateTransactionID(), amount, "credit", "Deposit");
            cout << "Added $" << amount << " to wallet. New balance: $" << balance << endl;
        } else {
            cout << "Invalid amount!" << endl;
        }
    }

    void spendFunds(double amount) {
        if (amount > 0 && amount <= balance) {
            cout << "Select category: 1. Food  2. Shopping  3. Bills  4. Entertainment\n";
            int catChoice;
            string category;
            cin >> catChoice;
            switch (catChoice) {
                case 1: category = "Food"; break;
                case 2: category = "Shopping"; break;
                case 3: category = "Bills"; break;
                case 4: category = "Entertainment"; break;
                default: category = "Other"; break;
            }

            balance -= amount;
            history.addTransaction(generateTransactionID(), amount, "debit", category);
            rewardPoints += (int)(amount / 10);
            cout << "Spent $" << amount << " on " << category << ". New balance: $" << balance << endl;
            cout << "Reward Points Earned: " << rewardPoints << endl;
        } else {
            cout << "Insufficient funds or invalid amount!" << endl;
        }
    }

    void redeemRewards() {
        if (rewardPoints >= 10) {
            double bonus = rewardPoints / 10.0;
            balance += bonus;
            cout << "Redeemed " << rewardPoints << " points for $" << bonus << endl;
            rewardPoints = 0;
        } else {
            cout << "Not enough reward points to redeem!" << endl;
        }
    }

    void showTransactionHistory() const {
        history.displayHistory();
    }

    void showFilteredTransactionHistory(const string& type) const {
        history.displayFilteredHistory(type);
    }

    void changeWalletPin() {
        changePin();
    }

    void displayInfo() const override {
        User::displayInfo();
        cout << "Wallet Balance: $" << balance << endl;
        cout << "Reward Points: " << rewardPoints << endl;
    }
};

// Main function
int main() {
    Wallet userWallet("Sunny_Suryaansh", "sunny_suryaansh@example.com");

    if (!userWallet.authenticate()) {
        cout << "Incorrect PIN. Access Denied!" << endl;
        return 1;
    }

    int choice;
    do {
        cout << "\n===== Digital Wallet System =====\n";
        cout << "1. Add Funds\n2. Spend Funds\n3. Show Wallet Info\n4. Show Transaction History\n";
        cout << "5. Show Only Credits\n6. Show Only Debits\n7. Redeem Reward Points\n";
        cout << "8. Change PIN\n9. Exit\n\nEnter choice: ";
        cin >> choice;

        double amount;

        switch (choice) {
            case 1:
                cout << "Enter amount to add: ";
                cin >> amount;
                userWallet.addFunds(amount);
                break;
            case 2:
                cout << "Enter amount to spend: ";
                cin >> amount;
                userWallet.spendFunds(amount);
                break;
            case 3:
                userWallet.displayInfo();
                break;
            case 4:
                userWallet.showTransactionHistory();
                break;
            case 5:
                userWallet.showFilteredTransactionHistory("credit");
                break;
            case 6:
                userWallet.showFilteredTransactionHistory("debit");
                break;
            case 7:
                userWallet.redeemRewards();
                break;
            case 8:
                userWallet.changeWalletPin();
                break;
            case 9:
                cout << "Exiting Digital Wallet System. Goodbye!\n";
                break;
            default:
                cout << "Invalid choice! Try again.\n";
        }
    } while (choice != 9);

    return 0;
}
