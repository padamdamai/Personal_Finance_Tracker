#include <iostream>
#include <pqxx/pqxx>

using namespace std;

// ---------------- REGISTER ----------------
void registerUser(pqxx::connection &C)
{
    string username, password;
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;

    pqxx::work W(C);
    W.exec_params("INSERT INTO users(username,password_hash) VALUES($1,$2)", username, password);
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

    pqxx::work W(C);
    pqxx::result R = W.exec_params(
        "SELECT user_id FROM users WHERE username=$1 AND password_hash=$2",
        username, password
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

    cout << "Date (YYYY-MM-DD): ";
    cin >> date;

    pqxx::work W(C);
    W.exec_params(
        "INSERT INTO transactions(user_id,type,amount,category,description,transaction_date) "
        "VALUES($1,'expense',$2,$3,$4,$5)",
        user_id, amount, category, description, date
    );
    W.commit();

    cout << "Expense added!\n";
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
             << " | " << row[4].c_str()
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
        cout << "8 Logout\n";
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
            case 8: return;
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