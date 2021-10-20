#pragma once
#include <QApplication>
#include <memory>

class MainWidget;

#define oApp (static_cast<Application*>(QCoreApplication::instance()))

namespace App
{
enum class AppState
{
    LoginForm,
    RegistrationForm,
    ChatWindowForm
};
}

class Application : public QApplication
{
public:
    Application(int argc, char** argv);
    void create();
    void show();
    void setAppState(App::AppState appState);

private:
    std::unique_ptr<MainWidget> _mainWidget;
    App::AppState               _appState;
};
