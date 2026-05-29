#include <iostream>
#include "Report.h"
#include <fstream>
#include "Security.h"
#include "Validation.h"

using namespace std;

Report::Report(
    pqxx::connection &conn
) : C(conn) {}

void Report::viewBalance(int user_id)
{
    pqxx::work W(C);

    pqxx::result R = W.exec_params(
        "SELECT "
        "COALESCE(SUM(CASE "
        "WHEN type='income' "
        "THEN amount ELSE 0 END),0)"
        "-"
        "COALESCE(SUM(CASE "
        "WHEN type='expense' "
        "THEN amount ELSE 0 END),0)"
        "FROM transactions "
        "WHERE user_id=$1",
        user_id
    );

    cout << "Balance: "
         << R[0][0].as<double>()
         << endl;
}

void Report::monthlyReport(int user_id)
{
    pqxx::work W(C);

    pqxx::result R = W.exec_params(
        "SELECT type,SUM(amount) "
        "FROM transactions "
        "WHERE user_id=$1 "
        "GROUP BY type",
        user_id
    );

    double income = 0;
    double expense = 0;

    for(auto row : R)
    {
        if(row[0].as<string>() == "income")
            income = row[1].as<double>();
        else
            expense = row[1].as<double>();
    }

    cout << "Income: " << income << endl;
    cout << "Expense: " << expense << endl;
    cout << "Savings: "
         << income - expense
         << endl;
}
// ---------------- EXPORT CSV ----------------
void Report::exportCSV(int user_id)
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
        string desc = Security::encryptDecrypt(row[4].c_str());

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

    cout << "CSV Exported Successfully as report.csv\n";

    char choice;

    cout << "Do you want to see graph of report? (y/n): ";
    cin >> choice;

    if(choice == 'y' || choice == 'Y')
    {
        cout << "Opening graph...\n";
        system("python3 graph.py");
    }
}

void Report::categoryExpense(int user_id)
{
    pqxx::work W(C);

    pqxx::result R = W.exec_params(
        "SELECT category, SUM(amount) "
        "FROM transactions "
        "WHERE user_id=$1 "
        "AND type='expense' "
        "GROUP BY category",
        user_id
    );

    cout << "\n--- Category Expense ---\n";

    for(auto row : R)
    {
        cout << row[0].c_str()
             << " : "
             << row[1].as<double>()
             << endl;
    }
}

void Report::setBudget(int user_id)
{
    string category;
    double limit;

    cout << "Enter category: ";
    getline(cin, category);

    cout << "Enter budget limit: ";
    limit = Validation::inputAmount();

    pqxx::work W(C);

    W.exec_params(
        "INSERT INTO budgets(user_id,category,limit_amount) "
        "VALUES($1,$2,$3)",
        user_id,
        category,
        limit
    );

    W.commit();

    cout << "Budget Set Successfully!\n";
}