#pragma once
#include <QDebug>
#include <QGuiApplication>
#include <QWidget>
#include <QWindow>
#include <memory>
#include <vector>

class MainWidget : public QWidget
{
    Q_OBJECT
public:
    MainWidget();
    std::int32_t addWidget(std::unique_ptr<QWidget> widget);
    /// Brief Hide old widget and show a new one
    void setCentralWidget(const std::int32_t widgetIndex);

private:
    std::int32_t                          _current = -1;
    std::unique_ptr<QWidget>              _body;
    std::vector<std::unique_ptr<QWidget>> _widgets;
};
