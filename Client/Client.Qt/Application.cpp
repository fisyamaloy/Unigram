#include "Application.hpp"

#include "MainWidget.hpp"

Application::Application(int argc, char** argv) : QApplication(argc, argv) {}

void Application::create()
{
    setOrganizationName("Course work");
    setApplicationName("Unigram");

    _mainWidget = std::make_unique<MainWidget>();
}

void Application::show() { _mainWidget->show(); }

void Application::setAppState(const App::AppState appState)
{
    _appState = appState;
    switch (_appState)
    {
        case App::AppState::LoginForm:
        {
            _mainWidget->setCentralWidget(0);
            break;
        }
        case App::AppState::RegistrationForm:
        {
            _mainWidget->setCentralWidget(1);
            break;
        }
        case App::AppState::ChatWindowForm:
        {
            _mainWidget->setCentralWidget(2);
            break;
        }
    }
}
