#ifndef SECURITY_H
#define SECURITY_H

#include <openssl/sha.h>
#include <sstream>
#include <iomanip>

using namespace std;

class Security
{
public:

    static string encryptDecrypt(string text)
    {
        char key = 'K';

        for(int i = 0; i < text.size(); i++)
        {
            text[i] = text[i] ^ key;
        }

        return text;
    }

    static string hashPassword(const string &password)
    {
        unsigned char hash[SHA256_DIGEST_LENGTH];

        SHA256(
            (unsigned char*)password.c_str(),
            password.size(),
            hash
        );

        stringstream ss;

        for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        {
            ss << hex
               << setw(2)
               << setfill('0')
               << (int)hash[i];
        }

        return ss.str();
    }
};

#endif