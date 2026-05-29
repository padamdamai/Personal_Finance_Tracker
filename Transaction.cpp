#include <iostream>
#include "Transaction.h"
#include "Validation.h"
#include "Security.h"

using namespace std;

Transaction::Transaction(
    pqxx::connection &conn
) : C(conn) {}

void Transaction::addIncome(int user_id)
{
    addTransaction(user_id, "income");
}

void Transaction::addExpense(int user_id)
{
    addTransaction(user_id, "expense");
}

void Transaction::addTransaction(
    int user_id,
    string type
)
{
    double amount;
    string category, description, date;

    cout << "Amount: ";
    amount = Validation::inputAmount();

    cout << "Category: ";
    getline(cin, category);

    cout << "Description: ";
    getline(cin, description);

    description =
        Security::encryptDecrypt(description);

    while(true)
    {
        cout << "Date (YYYY-MM-DD): ";
        getline(cin, date);

        if(Validation::isValidDate(date))
            break;

        cout << "Invalid date. Try again.\n";
    }

    try
    {
        pqxx::work W(C);

        W.exec_params(
            "INSERT INTO transactions("
            "user_id,type,amount,"
            "category,description,transaction_date)"
            "VALUES($1,$2,$3,$4,$5,$6)",
            user_id,
            type,
            amount,
            category,
            description,
            date
        );

        W.commit();

        cout << type
             << " added successfully!\n";
    }
    catch(const exception &e)
    {
        cout << "Database Error: "
             << e.what()
             << endl;
    }
}

void Transaction::viewTransactions(int user_id)
{
    pqxx::work W(C);

    pqxx::result R = W.exec_params(
        "SELECT transaction_id,type,amount,"
        "category,description,transaction_date "
        "FROM transactions "
        "WHERE user_id=$1",
        user_id
    );

    cout << "\n--- Transactions ---\n";

    for(auto row : R)
    {
        cout << "ID: "
             << row[0].as<int>()
             << " | "
             << row[1].c_str()
             << " | "
             << row[2].as<double>()
             << " | "
             << row[3].c_str()
             << " | "
             << Security::encryptDecrypt(
                    row[4].c_str()
                )
             << " | "
             << row[5].c_str()
             << endl;
    }
}

void Transaction::updateTransaction(int user_id)
{
    int id;
    double amount;

    cout << "Enter Transaction ID: ";
    id = Validation::inputChoice();

    cout << "Enter new amount: ";
    amount = Validation::inputAmount();

    pqxx::work W(C);

    W.exec_params(
        "UPDATE transactions "
        "SET amount=$1 "
        "WHERE transaction_id=$2 "
        "AND user_id=$3",
        amount,
        id,
        user_id
    );

    W.commit();

    cout << "Transaction updated!\n";
}

void Transaction::deleteTransaction(int user_id)
{
    int id;

    cout << "Enter Transaction ID: ";
    id = Validation::inputChoice();

    pqxx::work W(C);

    W.exec_params(
        "DELETE FROM transactions "
        "WHERE transaction_id=$1 "
        "AND user_id=$2",
        id,
        user_id
    );

    W.commit();

    cout << "Transaction deleted!\n";
}