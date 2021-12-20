#include "App.hpp"

#include "ServerInfo.hpp"

App::App() { connectToServer(ServerInfo::address, ServerInfo::port); }

App::~App()
{
    if (isConnected())
    {
        disconnectFromServer();
        std::cout << "Server Down\n";
    }
}

void App::onServerAccepted() { std::cout << "Server Accepted Connection\n"; }

void App::onServerPing(double timestamp) { std::cout << "Ping: " << timestamp << "\n"; }

void App::onRegistrationAnswer(Utility::RegistrationCodes registrationCode)
{
    if (registrationCode == Utility::RegistrationCodes::SUCCESS)
    {
        std::cout << "User was added" << std::endl;
    }
    else if (registrationCode == Utility::RegistrationCodes::LOGIN_ALREADY_EXISTS)
    {
        std::cout << "User with such login already exists" << std::endl;
    }
    else if (registrationCode == Utility::RegistrationCodes::EMAIL_ALREADY_EXISTS)
    {
        std::cout << "User with such email already exists" << std::endl;
    }
    else
    {
        std::cout << "Unknown RegistrationCode" << std::endl;
    }
}
