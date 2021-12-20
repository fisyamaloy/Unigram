#pragma once
#include <Network/Primitives.hpp>
#include <Utility/Utility.hpp>
#include <string>
#include <vector>

namespace DataAccess
{
class IRepository
{
public:
    virtual std::uint64_t              loginUser(const std::string& login, const std::string& pwdHash) = 0;
    virtual Utility::RegistrationCodes registerUser(const Network::RegistrationInfo& ri)               = 0;

    virtual ~IRepository() = default;
};
}  // namespace DataAccess
