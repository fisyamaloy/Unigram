#include "UsersAmountFinder.hpp"

#include <Network/Primitives.hpp>

std::uint16_t UsersAmountFinder::findUsersAmountWithSameTableAttribute(const std::string& condition)
{
    pqxx::work worker(*con);
    const auto        RECORDS_AMOUNT = worker.exec("SELECT COUNT(*) FROM users WHERE " + condition);
    return RECORDS_AMOUNT[0][0].as<std::uint16_t>();
}

std::uint16_t UsersAmountFinder::findUsersAmountWithSameLogin(const std::string& login)
{
    return findUsersAmountWithSameTableAttribute("login='" + login + "'");
}

std::uint16_t UsersAmountFinder::findUsersAmountWithSameEmail(const std::string& email)
{
    return findUsersAmountWithSameTableAttribute("email='" + email + "'");
}
