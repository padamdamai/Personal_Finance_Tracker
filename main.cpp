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

    try {
        pqxx::work W(C);

        W.exec("INSERT INTO users(username,password_hash) VALUES('"+
               username+"','"+password+"')");

        W.commit();

        cout << "Registration Successful!\n";
    }
    catch (const exception &e) {
        cout << "Error: " << e.what() << endl;
    }
}

// ---------------- LOGIN ----------------
bool loginUser(pqxx::connection &C)
{
    string username, password;

    cout << "Enter username: ";
    cin >> username;

    cout << "Enter password: ";
    cin >> password;

    try {
        pqxx::work W(C);

        pqxx::result R = W.exec(
            "SELECT * FROM users WHERE username='" + username +
            "' AND password_hash='" + password + "'"
        );

        if (R.size() > 0) {
            cout << "Login Successful!\n";
            return true;
        } else {
            cout << "Invalid Username or Password!\n";
            return false;
        }
    }
    catch (const exception &e) {
        cout << "Error: " << e.what() << endl;
        return false;
    }
}

// ---------------- MAIN MENU ----------------
int main()
{
    try {
        pqxx::connection C("dbname=finance_tracker user=postgres password=helloworld");

        if (!C.is_open()) {
            cout << "Cannot open database\n";
            return 1;
        }

        int choice;

        while (true)
        {
            cout << "\n===== Personal Finance Tracker =====\n";
            cout << "1. Register\n";
            cout << "2. Login\n";
            cout << "3. Exit\n";
            cout << "Enter choice: ";
            cin >> choice;

            switch (choice)
            {
                case 1:
                    registerUser(C);
                    break;

                case 2:
                    loginUser(C);
                    break;

                case 3:
                    cout << "Exiting...\n";
                    return 0;

                default:
                    cout << "Invalid choice!\n";
            }
        }
    }
    catch (const exception &e) {
        cout << e.what() << endl;
    }

    return 0;
}