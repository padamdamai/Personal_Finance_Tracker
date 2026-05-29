#ifndef VALIDATION_H
#define VALIDATION_H

#include <iostream>
#include <regex>
#include <limits>

using namespace std;

class Validation
{
public:

    static bool isValidDate(string date)
    {
        regex pattern("^\\d{4}-\\d{2}-\\d{2}$");

        if (!regex_match(date, pattern))
            return false;

        int year = stoi(date.substr(0,4));
        int month = stoi(date.substr(5,2));
        int day = stoi(date.substr(8,2));

        if(month < 1 || month > 12)
            return false;

        int daysInMonth[] =
        {31,28,31,30,31,30,31,31,30,31,30,31};

        if((year % 4 == 0 && year % 100 != 0)
            || year % 400 == 0)
        {
            daysInMonth[1] = 29;
        }

        if(day < 1 || day > daysInMonth[month - 1])
            return false;

        return true;
    }

    static double inputAmount()
    {
        double amount;

        while(true)
        {
            cin >> amount;

            if(cin.fail() || amount <= 0)
            {
                cout << "Invalid amount. Try again: ";

                cin.clear();

                cin.ignore(
                    numeric_limits<streamsize>::max(),
                    '\n'
                );
            }
            else
            {
                cin.ignore(
                    numeric_limits<streamsize>::max(),
                    '\n'
                );

                return amount;
            }
        }
    }

    static int inputChoice()
    {
        int choice;

        while(true)
        {
            cin >> choice;

            if(cin.fail())
            {
                cout << "Invalid input. Enter number: ";

                cin.clear();

                cin.ignore(
                    numeric_limits<streamsize>::max(),
                    '\n'
                );
            }
            else
            {
                cin.ignore(
                    numeric_limits<streamsize>::max(),
                    '\n'
                );

                return choice;
            }
        }
    }
};

#endif