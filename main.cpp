#include <iostream>
#include <pqxx/pqxx>
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>
#include <fstream>

using namespace std;

// ---------------- SIMPLE ENCRYPTION ----------------
string encryptDecrypt(string text)
{
    char key = 'K'; // secret key

    for (int i = 0; i < text.size(); i++)
        text[i] = text[i] ^ key;

    return text;
}

// ---------------- HASH FUNCTION ----------------
string hashPassword(const string &password)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)password.c_str(), password.size(), hash);

    stringstream ss;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        ss << hex << setw(2) << setfill('0') << (int)hash[i];

    return ss.str();
}

// ---------------- REGISTER ----------------
void registerUser(pqxx::connection &C)
{
    string username, password;
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;

    string hashed = hashPassword(password);  

    pqxx::work W(C);
    W.exec_params("INSERT INTO users(username,password_hash) VALUES($1,$2)", username, hashed);
    W.commit();

    cout << "Registration Successful!\n";
}

// ---------------- LOGIN ----------------
bool loginUser(pqxx::connection &C, int &user_id)
{
    string username, password;

    cout << "Enter username: ";
    cin >> username;

    cout << "Enter password: ";
    cin >> password;

    string hashed = hashPassword(password); 

    pqxx::work W(C);
    pqxx::result R = W.exec_params(
        "SELECT user_id FROM users WHERE username=$1 AND password_hash=$2",
        username, hashed
    );

    if (R.size() == 1)
    {
        user_id = R[0][0].as<int>();
        cout << "Login Successful!\n";
        return true;
    }
    else
    {
        cout << "Invalid credentials\n";
        return false;
    }
}

// ---------------- ADD INCOME ----------------
void addIncome(pqxx::connection &C, int user_id)
{
    double amount;
    string category, description, date;

    cout << "Amount: ";
    cin >> amount;

    cout << "Category: ";
    cin >> category;

    cin.ignore();
    cout << "Description: ";
    getline(cin, description);
    description = encryptDecrypt(description);

    cout << "Date (YYYY-MM-DD): ";
    cin >> date;

    pqxx::work W(C);
    W.exec_params(
        "INSERT INTO transactions(user_id,type,amount,category,description,transaction_date) "
        "VALUES($1,'income',$2,$3,$4,$5)",
        user_id, amount, category, description, date
    );
    W.commit();

    cout << "Income added!\n";
}

// ---------------- ADD EXPENSE ----------------
void addExpense(pqxx::connection &C, int user_id)
{
    double amount;
    string category, description, date;

    cout << "Amount: ";
    cin >> amount;

    cout << "Category: ";
    cin >> category;

    cin.ignore();
    cout << "Description: ";
    getline(cin, description);
    description = encryptDecrypt(description);

    cout << "Date (YYYY-MM-DD): ";
    cin >> date;

    pqxx::work W(C);
    W.exec_params(
        "INSERT INTO transactions(user_id,type,amount,category,description,transaction_date) "
        "VALUES($1,'expense',$2,$3,$4,$5)",
        user_id, amount, category, description, date
    );

    // BUDGET CHECK
    pqxx::result R = W.exec_params(
        "SELECT limit_amount FROM budgets WHERE user_id=$1 AND category=$2",
        user_id, category
    );

    if (R.size() == 1)
    {
        double limit = R[0][0].as<double>();

        pqxx::result spent = W.exec_params(
            "SELECT COALESCE(SUM(amount),0) FROM transactions "
            "WHERE user_id=$1 AND category=$2 AND type='expense'",
            user_id, category
        );

        double total = spent[0][0].as<double>();

        if (total > limit)
            cout << "⚠ Budget exceeded for " << category << "!\n";
    }

    W.commit();

    cout << "Expense added!\n";
}

// ---------------- CATEGORY EXPENSE ----------------
void categoryExpense(pqxx::connection &C, int user_id)
{
    pqxx::work W(C);

    pqxx::result R = W.exec_params(
        "SELECT category, SUM(amount) FROM transactions "
        "WHERE user_id=$1 AND type='expense' GROUP BY category",
        user_id
    );

    cout << "\n--- Category Expenses ---\n";

    for (auto row : R)
    {
        cout << row[0].c_str() << " : " << row[1].as<double>() << endl;
    }
}

// ---------------- SET BUDGET ----------------
void setBudget(pqxx::connection &C, int user_id)
{
    string category;
    double limit;

    cout << "Enter category: ";
    cin >> category;

    cout << "Enter budget limit: ";
    cin >> limit;

    pqxx::work W(C);

    W.exec_params(
        "INSERT INTO budgets(user_id,category,limit_amount) VALUES($1,$2,$3)",
        user_id, category, limit
    );

    W.commit();

    cout << "Budget set successfully!\n";
}

// ---------------- VIEW TRANSACTIONS ----------------
void viewTransactions(pqxx::connection &C, int user_id)
{
    pqxx::work W(C);

    pqxx::result R = W.exec_params(
        "SELECT transaction_id, type, amount, category, description, transaction_date "
        "FROM transactions WHERE user_id=$1 ORDER BY transaction_date",
        user_id
    );

    cout << "\n--- Transactions ---\n";

    for (auto row : R)
    {
        cout << "ID: " << row[0].as<int>()
             << " | " << row[1].c_str()
             << " | " << row[2].as<double>()
             << " | " << row[3].c_str()
             << " | " << encryptDecrypt(row[4].c_str())
             << " | " << row[5].c_str()
             << endl;
    }
}

// ---------------- UPDATE TRANSACTION ----------------
void updateTransaction(pqxx::connection &C, int user_id)
{
    int id;
    double amount;

    cout << "Enter Transaction ID to update: ";
    cin >> id;

    cout << "Enter new amount: ";
    cin >> amount;

    pqxx::work W(C);

    W.exec_params(
        "UPDATE transactions SET amount=$1 WHERE transaction_id=$2 AND user_id=$3",
        amount, id, user_id
    );

    W.commit();

    cout << "Transaction updated!\n";
}

// ---------------- DELETE TRANSACTION ----------------
void deleteTransaction(pqxx::connection &C, int user_id)
{
    int id;

    cout << "Enter Transaction ID to delete: ";
    cin >> id;

    pqxx::work W(C);

    W.exec_params(
        "DELETE FROM transactions WHERE transaction_id=$1 AND user_id=$2",
        id, user_id
    );

    W.commit();

    cout << "Transaction deleted!\n";
}

// ---------------- VIEW BALANCE ----------------
void viewBalance(pqxx::connection &C, int user_id)
{
    pqxx::work W(C);

    pqxx::result R = W.exec_params(
        "SELECT "
        "COALESCE(SUM(CASE WHEN type='income' THEN amount ELSE 0 END),0) - "
        "COALESCE(SUM(CASE WHEN type='expense' THEN amount ELSE 0 END),0) "
        "FROM transactions WHERE user_id=$1",
        user_id
    );

    cout << "Balance: " << R[0][0].as<double>() << endl;
}

// ---------------- MONTHLY REPORT ----------------
void monthlyReport(pqxx::connection &C, int user_id)
{
    pqxx::work W(C);

    pqxx::result R = W.exec_params(
        "SELECT type, SUM(amount) FROM transactions "
        "WHERE user_id=$1 AND DATE_TRUNC('month', transaction_date)=DATE_TRUNC('month', CURRENT_DATE) "
        "GROUP BY type",
        user_id
    );

    double income = 0, expense = 0;

    for (auto row : R)
    {
        if (row[0].as<string>() == "income")
            income = row[1].as<double>();
        else
            expense = row[1].as<double>();
    }

    cout << "Income: " << income << "\nExpense: " << expense
         << "\nSavings: " << income - expense << endl;
}

// ---------------- EXPORT CSV ----------------
void exportCSV(pqxx::connection &C, int user_id)
{
    pqxx::work W(C);

    pqxx::result R = W.exec_params(
        "SELECT transaction_date, type, amount, category, description "
        "FROM transactions WHERE user_id=$1",
        user_id
    );

    ofstream file("report.csv");

    file << "Date,Type,Amount,Category,Description\n";

    for (auto row : R)
    {
        string desc = encryptDecrypt(row[4].c_str());
        
        // Escape quotes inside description
        for (size_t i = 0; i < desc.size(); i++)
        {
            if (desc[i] == '"')
                desc.insert(i++, "\"");
        }
        
        file << row[0].c_str() << ","
            << row[1].c_str() << ","
            << row[2].as<double>() << ","
            << row[3].c_str() << ","
            << "\"" << desc << "\"" << "\n";
        
    }

    file.close();

    cout << "CSV Exported as report.csv\n";
}

// ---------------- DASHBOARD ----------------
void dashboard(pqxx::connection &C, int user_id)
{
    int choice;

    while (true)
    {
        cout << "\n===== DASHBOARD =====\n";
        cout << "1 Add Income\n";
        cout << "2 Add Expense\n";
        cout << "3 View Transactions\n";
        cout << "4 Update Transaction\n";
        cout << "5 Delete Transaction\n";
        cout << "6 View Balance\n";
        cout << "7 Monthly Report\n";
        cout << "8 Category Expense\n"; 
        cout << "9 Set Budget\n";         
        cout << "10 Logout\n";
        cout << "11 Export CSV\n";
        cout << "12 Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice)
        {
            case 1: addIncome(C, user_id); break;
            case 2: addExpense(C, user_id); break;
            case 3: viewTransactions(C, user_id); break;
            case 4: updateTransaction(C, user_id); break;
            case 5: deleteTransaction(C, user_id); break;
            case 6: viewBalance(C, user_id); break;
            case 7: monthlyReport(C, user_id); break;
            case 8: categoryExpense(C, user_id); break;
            case 9: setBudget(C, user_id); break;
            case 10: return;
            case 11: exportCSV(C, user_id); break;
            case 12: exit(0);
            default: cout << "Invalid choice\n";
        }
    }
}

// ---------------- MAIN ----------------
int main()
{
    pqxx::connection C("dbname=finance_tracker user=postgres password=helloworld");

    int choice, user_id;

    while (true)
    {
        cout << "\n1 Register\n2 Login\n3 Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice)
        {
            case 1: registerUser(C); break;
            case 2:
                if (loginUser(C, user_id))
                    dashboard(C, user_id);
                break;
            case 3: return 0;
        }
    }
}