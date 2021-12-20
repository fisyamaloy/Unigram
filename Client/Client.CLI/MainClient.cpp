#include <chrono>
#include <future>
#include <iostream>
#include <string>
#include <queue>

#include "App.hpp"
#include "Utility/UserDataValidation.hpp"

static std::queue<std::string> queriesQueue;

std::string GetLineFromCin()
{
    std::string line;
    if (queriesQueue.empty())
    {
        std::getline(std::cin, line);

        queriesQueue.push(std::move(line));
    }

    line = queriesQueue.front();
    queriesQueue.pop();

    return line;
}

int main(int argc, char** argv)
{
    for (std::size_t argIndex = 1; argIndex < static_cast<std::size_t>(argc); argIndex++)
    {
        queriesQueue.emplace(argv[argIndex]);
    }

    queriesQueue.emplace("cl");

    App  clientApp;

    while (!clientApp.isConnected());

    bool quit   = false;
    auto future = std::async(std::launch::async, GetLineFromCin);

    while (!quit)
    {
        if (future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
        {
            static std::string cmd;
            cmd = future.get();

            if (cmd == "p")
            {
                clientApp.pingServer();
                cmd = "";
            }
            else if (cmd == "ur")
            {
                clientApp.userRegistration("danil_fisiuk@epam.com", "tuchko321", "19967340");
            }
            
            else if (cmd == "q")
            {
                quit = true;
                clientApp.disconnectFromServer();
                continue;
            }
            future = std::async(std::launch::async, GetLineFromCin);
        }
    }

    return EXIT_SUCCESS;
}
