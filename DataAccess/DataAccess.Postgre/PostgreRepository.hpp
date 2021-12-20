#pragma once

#include <DataAccess/IRepository.hpp>
#include <Network/Primitives.hpp>
#include <iostream>
#include <pqxx/pqxx>
#include <string>

#include "DataBaseOptions.hpp"
#include "Utility/Exception.hpp"
#include "Utility/Utility.hpp"

namespace DataAccess
{
class PostgreRepository : public IRepository
{
    pqxx::connection* con;

public:
    PostgreRepository() : con(new pqxx::connection(pqxx::zview(DBOptions::real))){};

    virtual ~PostgreRepository() = default;

    std::uint64_t              loginUser(const std::string& login, const std::string& pwdHash) override final;
    Utility::RegistrationCodes registerUser(const Network::RegistrationInfo& ri) override final;
};
}  // namespace DataAccess
