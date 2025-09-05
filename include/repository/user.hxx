#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma once

#include <memory>
#include <string>
#include <odb/core.hxx>
using namespace std;

#pragma db object

class User {
public:
    User() {}
    User(const string& email, const string& name, const string& password) : email(email), name(name), password(password) {}

    friend class odb::access;

    #pragma db id auto
    unsigned long long id;

    #pragma db type("VARCHAR(255)") unique
    string email;

    #pragma db type("VARCHAR(255)")
    string name;

    #pragma db type("VARCHAR(255)")
    string password;
};