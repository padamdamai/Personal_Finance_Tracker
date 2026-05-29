#ifndef REPORT_H
#define REPORT_H

#include <pqxx/pqxx>

class Report
{
private:
    pqxx::connection &C;

public:

    Report(pqxx::connection &conn);

    void viewBalance(int user_id);

    void monthlyReport(int user_id);

    void exportCSV(int user_id);

    void categoryExpense(int user_id);

    void setBudget(int user_id);
};

#endif