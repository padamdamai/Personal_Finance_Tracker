#ifndef HIDDENPASSWORD_H
#define HIDDENPASSWORD_H

#include <iostream>
#include <termios.h>
#include <unistd.h>

using namespace std;

class HiddenPassword
{
public:

    static string inputPassword()
    {
        string password;
        char ch;

        termios oldt, newt;

        tcgetattr(STDIN_FILENO, &oldt);

        newt = oldt;

        newt.c_lflag &= ~(ECHO);

        tcsetattr(STDIN_FILENO, TCSANOW, &newt);

        while (true)
        {
            ch = getchar();

            if (ch == '\n')
                break;

            if (ch == 127)
            {
                if (!password.empty())
                {
                    password.pop_back();
                    cout << "\b \b";
                }
            }
            else
            {
                password += ch;
                cout << "*";
            }
        }

        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

        cout << endl;

        return password;
    }
};

#endif