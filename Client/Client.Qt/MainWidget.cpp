#include <MainWidget.hpp>

MainWidget::MainWidget() : QWidget(nullptr)
{
    window()->createWinId();
    setMinimumWidth(1024);
    setMinimumHeight(768);
    _body = std::make_unique<QWidget>();
}

std::int32_t MainWidget::addWidget(std::unique_ptr<QWidget> widget)
{
    widget->setParent(_body.get());
    widget->hide();
    widget->resize(_body->width(), _body->height());
    _widgets.push_back(std::move(widget));
    return std::int32_t(_widgets.size()) - 1;
}

void MainWidget::setCentralWidget(const std::int32_t index)
{
    if (index >= 0 && index < std::int32_t(_widgets.size()))
    {
        _widgets[index]->resize(_body->width(), _body->height());
        _widgets[index]->show();
        if (_current >= 0) _widgets[_current]->hide();
        _current = index;
    }
}
