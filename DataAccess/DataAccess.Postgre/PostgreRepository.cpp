#include "PostgreRepository.hpp"

#include "UsersAmountFinder.hpp"

using namespace DataAccess;

std::uint64_t PostgreRepository::loginUser(const std::string& login, const std::string& pwdHas)
{
    static pqxx::work worker(*con);

    pqxx::result response = worker.exec("SELECT id FROM users WHERE login='" + login + "' AND password_hash='" + pwdHas + "'");
    return response.empty() ? 0 : response[0][0].as<std::uint64_t>();
}

Utility::RegistrationCodes PostgreRepository::registerUser(const Network::RegistrationInfo& ri)
{
    pqxx::work worker(*con);

    UsersAmountFinder finder;
    // Check on existing of login and email in repository.
    if (finder.findUsersAmountWithSameEmail(ri.email) > 0)
    {
        return Utility::RegistrationCodes::EMAIL_ALREADY_EXISTS;
    }
    if (finder.findUsersAmountWithSameLogin(ri.login) > 0)
    {
        return Utility::RegistrationCodes::LOGIN_ALREADY_EXISTS;
    }

    // Insert new user.
    worker.exec("INSERT INTO users (email, login, password_hash) VALUES ('" + ri.email + "', '" 
                    + ri.login + "', '" + ri.passwordHash + "')");
    worker.commit();

    return Utility::RegistrationCodes::SUCCESS;
}
