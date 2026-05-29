#include "User.h"
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include "Security.h"



using namespace std;

User::User(pqxx::connection &conn) : C(conn) {}

void User::registerUser()
{
    string username;
    string password;
    string confirmPassword;

    cout << "Enter username: ";
    getline(cin, username);

    cout << "Enter password: ";
    password = getHiddenPassword();

    cout << "Confirm password: ";
    confirmPassword = getHiddenPassword();

    if(password != confirmPassword)
    {
        cout << "Passwords do not match!\n";
        return;
    }

    string hashed = Security::hashPassword(password);

    try
    {
        pqxx::work W(C);

        W.exec_params(
            "INSERT INTO users(username,password_hash) VALUES($1,$2)",
            username,
            hashed
        );

        W.commit();

        cout << "Registration Successful!\n";
    }
    catch(const exception &e)
    {
        cout << "Error: " << e.what() << endl;
    }
}

bool User::loginUser(int &user_id)
{
    string username;
    string password;

    cout << "Enter username: ";
    getline(cin, username);

    cout << "Enter password: ";
    password = getHiddenPassword();

    string hashed = Security::hashPassword(password);

    try
    {
        pqxx::work W(C);

        pqxx::result R = W.exec_params(
            "SELECT user_id FROM users WHERE username=$1 AND password_hash=$2",
            username,
            hashed
        );

        if(R.size() == 1)
        {
            user_id = R[0][0].as<int>();

            cout << "Login Successful!\n";

            return true;
        }
        else
        {
            cout << "Invalid Credentials\n";
            return false;
        }
    }
    catch(const exception &e)
    {
        cout << "Database Error: " << e.what() << endl;
        return false;
    }
}
// ---------------- HIDDEN PASSWORD INPUT ----------------
string getHiddenPassword()
{
    string password;

    char ch;

    termios oldt, newt;

    tcgetattr(STDIN_FILENO, &oldt);

    newt = oldt;

    // Disable terminal echo
    newt.c_lflag &= ~(ECHO);

    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    while(true)
    {
        ch = getchar();

        // ENTER pressed
        if(ch == '\n' || ch == '\r')
        {
            cout << endl;
            break;
        }

        // BACKSPACE support
        else if(ch == 127 || ch == 8)
        {
            if(!password.empty())
            {
                password.pop_back();

                cout << "\b \b";
            }
        }

        else
        {
            password += ch;

            // Show *
            cout << '*';
        }
    }

    // Restore terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return password;
}