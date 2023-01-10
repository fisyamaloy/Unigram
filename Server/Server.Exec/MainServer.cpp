#include <iostream>
#include <stdexcept>

#include <Server.hpp>

int main()
{
    try
    {
        Server::Server server(65003);
        server.start();

        while (true)
        {
            server.update();
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
