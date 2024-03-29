#include "ComboBox.hpp"

#include <Style/Styles.hpp>

ComboBox::ComboBox(QWidget* parent) : QComboBox(parent)
{
    setFont(st::semiboldFont);
    setStyleSheet(
        QString("QComboBox { "
                "border: 0px;"
                "background-color: rgba(0,0,0,0);"
                "color: white;"
                "}"));
    setContentsMargins(st::defaultMargins);
}
