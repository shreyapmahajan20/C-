#include <iostream>
#include <fstream>
#include <map>
#include <stdexcept>
using namespace std;

#define MIN_BALANCE 500

// Custom exception class
class InsufficientFunds : public runtime_error {
public:
    InsufficientFunds() : runtime_error("Error: Insufficient funds in the account!") {}
};

// Account class
class Account {
private:
    long accountNumber;
    string firstName;
    string lastName;
    float balance;
    static long NextAccountNumber;

public:
    Account() {}
    Account(string fname, string lname, float balance);

    long getAccNo() const { return accountNumber; }
    string getFirstName() const { return firstName; }
    string getLastName() const { return lastName; }
    float getBalance() const { return balance; }

    void Deposit(float amount);
    void Withdraw(float amount);

    static void setLastAccountNumber(long accountNumber);
    static long getLastAccountNumber();

    friend ofstream &operator<<(ofstream &ofs, const Account &acc);
    friend ifstream &operator>>(ifstream &ifs, Account &acc);
    friend ostream &operator<<(ostream &os, const Account &acc);
};

long Account::NextAccountNumber = 0;

// Bank class
class Bank {
private:
    map<long, Account> accounts; // Changed to std::map

public:
    Bank();
    Account OpenAccount(string fname, string lname, float balance);
    Account BalanceEnquiry(long accountNumber);
    Account Deposit(long accountNumber, float amount);
    Account Withdraw(long accountNumber, float amount);
    Account Transfer(long fromAccountNumber, long toAccountNumber, float amount);
    void CloseAccount(long accountNumber);
    void ShowAllAccounts() const;
    ~Bank();
};

// Main Function
int main() {
    Bank bank;
    Account acc;

    int choice;
    string fname, lname;
    long accountNumber, fromAccountNumber, toAccountNumber;
    float balance, amount;

    cout << "Welcome to the Banking System" << endl;
    do {
        cout << "\nMenu Options:";
        cout << "\n1. Open an Account";
        cout << "\n2. Balance Enquiry";
        cout << "\n3. Deposit";
        cout << "\n4. Withdraw";
        cout << "\n5. Close an Account";
        cout << "\n6. Show All Accounts";
        cout << "\n7. Quit";
        cout << "\n8. Transfer";
        cout << "\nEnter your choice: ";
        cin >> choice;

        try {
            switch (choice) {
                case 1:
                    cout << "Enter First Name: ";
                    cin >> fname;
                    cout << "Enter Last Name: ";
                    cin >> lname;
                    cout << "Enter Initial Balance: ";
                    cin >> balance;
                    acc = bank.OpenAccount(fname, lname, balance);
                    cout << "\nAccount Created Successfully!\n" << acc;
                    break;

                case 2:
                    cout << "Enter Account Number: ";
                    cin >> accountNumber;
                    acc = bank.BalanceEnquiry(accountNumber);
                    cout << "\nAccount Details:\n" << acc;
                    break;

                case 3:
                    cout << "Enter Account Number: ";
                    cin >> accountNumber;
                    cout << "Enter Deposit Amount: ";
                    cin >> amount;
                    acc = bank.Deposit(accountNumber, amount);
                    cout << "\nDeposit Successful!\n" << acc;
                    break;

                case 4:
                    cout << "Enter Account Number: ";
                    cin >> accountNumber;
                    cout << "Enter Withdrawal Amount: ";
                    cin >> amount;
                    acc = bank.Withdraw(accountNumber, amount);
                    cout << "\nWithdrawal Successful!\n" << acc;
                    break;

                case 5:
                    cout << "Enter Account Number: ";
                    cin >> accountNumber;
                    bank.CloseAccount(accountNumber);
                    cout << "\nAccount Closed Successfully!";
                    break;

                case 6:
                    bank.ShowAllAccounts();
                    break;

                case 7:
                    cout << "Exiting the Banking System. Thank you!" << endl;
                    break;

                case 8:
                    cout << "Enter Source Account Number: ";
                    cin >> fromAccountNumber;
                    cout << "Enter Destination Account Number: ";
                    cin >> toAccountNumber;
                    cout << "Enter Transfer Amount: ";
                    cin >> amount;
                    try {
                        acc = bank.Transfer(fromAccountNumber, toAccountNumber, amount);
                        cout << "\nTransfer Successful!\n" << acc;
                    } catch (const runtime_error &e) {
                        cout << "\nError: " << e.what() << endl;
                    }
                    break;

                default:
                    cout << "Invalid choice! Please select a valid option." << endl;
            }
        } catch (const runtime_error &e) {
            cout << "\nError: " << e.what() << endl;
        }
    } while (choice != 7);

    return 0;
}

// Account class methods
Account::Account(string fname, string lname, float balance)
    : firstName(fname), lastName(lname), balance(balance) {
    NextAccountNumber++;
    accountNumber = NextAccountNumber;
}

void Account::Deposit(float amount) {
    balance += amount;
}

void Account::Withdraw(float amount) {
    if (balance - amount < MIN_BALANCE) {
        throw InsufficientFunds();
    }
    balance -= amount;
}

void Account::setLastAccountNumber(long accountNumber) {
    NextAccountNumber = accountNumber;
}

long Account::getLastAccountNumber() {
    return NextAccountNumber;
}

ofstream &operator<<(ofstream &ofs, const Account &acc) {
    ofs << acc.accountNumber << endl
        << acc.firstName << endl
        << acc.lastName << endl
        << acc.balance << endl;
    return ofs;
}

ifstream &operator>>(ifstream &ifs, Account &acc) {
    ifs >> acc.accountNumber >> acc.firstName >> acc.lastName >> acc.balance;
    return ifs;
}

ostream &operator<<(ostream &os, const Account &acc) {
    os << "Account Number: " << acc.getAccNo() << endl
       << "First Name: " << acc.getFirstName() << endl
       << "Last Name: " << acc.getLastName() << endl
       << "Balance: " << acc.getBalance() << endl;
    return os;
}

// Bank class methods
Bank::Bank() {
    ifstream infile("Bank.data");
    if (!infile) return;

    Account acc;
    while (infile >> acc) {
        accounts[acc.getAccNo()] = acc;
    }
    infile.close();

    // Set the last account number if accounts exist
    if (!accounts.empty()) {
        Account::setLastAccountNumber(accounts.rbegin()->first);
    }
}

Account Bank::OpenAccount(string fname, string lname, float balance) {
    if (balance < MIN_BALANCE) {
        throw runtime_error("Initial balance must be at least " + to_string(MIN_BALANCE));
    }
    Account acc(fname, lname, balance);
    accounts[acc.getAccNo()] = acc;

    ofstream outfile("Bank.data", ios::trunc);
    for (const auto &pair : accounts) {
        outfile << pair.second;
    }
    outfile.close();
    return acc;
}

Account Bank::BalanceEnquiry(long accountNumber) {
    auto it = accounts.find(accountNumber);
    if (it == accounts.end()) {
        throw runtime_error("Account not found!");
    }
    return it->second;
}

Account Bank::Deposit(long accountNumber, float amount) {
    auto it = accounts.find(accountNumber);
    if (it == accounts.end()) {
        throw runtime_error("Account not found!");
    }
    it->second.Deposit(amount);
    return it->second;
}

Account Bank::Withdraw(long accountNumber, float amount) {
    auto it = accounts.find(accountNumber);
    if (it == accounts.end()) {
        throw runtime_error("Account not found!");
    }
    it->second.Withdraw(amount);
    return it->second;
}

Account Bank::Transfer(long fromAccountNumber, long toAccountNumber, float amount) {
    auto fromIt = accounts.find(fromAccountNumber);
    auto toIt = accounts.find(toAccountNumber);

    if (fromIt == accounts.end()) {
        throw runtime_error("Source account not found!");
    }
    if (toIt == accounts.end()) {
        throw runtime_error("Destination account not found!");
    }

    if (fromIt->second.getBalance() - amount < MIN_BALANCE) {
        throw InsufficientFunds();
    }
    fromIt->second.Withdraw(amount);
    toIt->second.Deposit(amount);

    return fromIt->second; // Return the updated source account, or another account if needed
}

void Bank::CloseAccount(long accountNumber) {
    auto it = accounts.find(accountNumber);
    if (it == accounts.end()) {
        throw runtime_error("Account not found!");
    }
    accounts.erase(it);
}

void Bank::ShowAllAccounts() const {
    if (accounts.empty()) {
        cout << "No accounts to display." << endl;
        return;
    }
    for (const auto &pair : accounts) {
        cout << pair.second << endl;
    }
}
Bank::~Bank() {
    ofstream outfile("Bank.data", ios::trunc);
    for (const auto &pair : accounts) {
        outfile << pair.second;
    }
    outfile.close();
}

