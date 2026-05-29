#include <iostream>
#include <pqxx/pqxx>

#include "User.h"
#include "Transaction.h"
#include "Report.h"
#include "Validation.h"

using namespace std;

int main()
{
    try
    {
        pqxx::connection C(
            "dbname=finance_tracker "
            "user=postgres "
            "password=helloworld "
            "host=localhost "
            "port=5432"
        );

        User user(C);
        Transaction transaction(C);
        Report report(C);

        int choice;
        int user_id;

        while(true)
        {
            cout << "\n1 Register\n";
            cout << "2 Login\n";
            cout << "3 Exit\n";

            cout << "Enter choice: ";

            choice =
                Validation::inputChoice();

            switch(choice)
            {
                case 1:
                    user.registerUser();
                    break;

                case 2:

                    if(user.loginUser(user_id))
                    {
                        while(true)
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

                            int dashboardChoice;

                            dashboardChoice =
                                Validation::inputChoice();

                            switch(dashboardChoice)
                                {
                                    case 1:
                                        transaction.addIncome(user_id);
                                        break;
                                
                                    case 2:
                                        transaction.addExpense(user_id);
                                        break;
                                
                                    case 3:
                                        transaction.viewTransactions(user_id);
                                        break;
                                
                                    case 4:
                                        transaction.updateTransaction(user_id);
                                        break;
                                
                                    case 5:
                                        transaction.deleteTransaction(user_id);
                                        break;
                                
                                    case 6:
                                        report.viewBalance(user_id);
                                        break;
                                
                                    case 7:
                                        report.monthlyReport(user_id);
                                        break;
                                
                                    case 8:
                                        report.categoryExpense(user_id);
                                        break;
                                
                                    case 9:
                                        report.setBudget(user_id);
                                        break;
                                
                                    case 10:
                                        goto logout;
                                
                                    case 11:
                                        report.exportCSV(user_id);
                                        break;
                                
                                    case 12:
                                        exit(0);
                                
                                    default:
                                        cout << "Invalid choice\n";
                                }
                        }

                        logout:
                        break;
                    }

                    break;

                case 3:
                    return 0;

                default:
                    cout << "Invalid choice\n";
            }
        }
    }
    catch(const exception &e)
    {
        cout << "Fatal Error: "
             << e.what()
             << endl;
    }

    return 0;
}