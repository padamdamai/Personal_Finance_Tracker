#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <pqxx/pqxx>

class Transaction
{
private:
    pqxx::connection &C;

public:

    Transaction(pqxx::connection &conn);

    void addIncome(int user_id);

    void addExpense(int user_id);

    void viewTransactions(int user_id);

    void updateTransaction(int user_id);

    void deleteTransaction(int user_id);

private:

    void addTransaction(
        int user_id,
        std::string type
    );
};

#endif