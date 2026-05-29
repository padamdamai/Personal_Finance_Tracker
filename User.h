#ifndef USER_H
#define USER_H
#include <string>

#include <pqxx/pqxx>

using namespace std;
class User
{
private:
    pqxx::connection &C;

public:
    User(pqxx::connection &conn);

    void registerUser();

    
    bool loginUser(int &user_id);
};


string getHiddenPassword();

#endif