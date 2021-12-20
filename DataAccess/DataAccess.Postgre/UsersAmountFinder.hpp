#pragma once
#include <Network/Primitives.hpp>

#include "DataBaseOptions.hpp"
#include <pqxx/pqxx>

class UsersAmountFinder
{
private:
    pqxx::connection* con;

public:
    UsersAmountFinder() : con(new pqxx::connection(pqxx::zview(DBOptions::real))) {}
    virtual ~UsersAmountFinder() = default;

    std::uint16_t findUsersAmountWithSameTableAttribute(const std::string& condition);
    std::uint16_t findUsersAmountWithSameLogin(const std::string& login);
    std::uint16_t findUsersAmountWithSameEmail(const std::string& email);
};
