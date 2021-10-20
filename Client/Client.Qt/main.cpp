#include <Application.hpp>

int main(int argc, char* argv[])
{
    Application* app = new Application(argc, argv);
    app->create();
    app->show();

    return app->exec();
}

