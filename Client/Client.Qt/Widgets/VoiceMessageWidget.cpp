#include "VoiceMessageWidget.hpp"

#include <QPainter>
#include <QResizeEvent>
#include <QWidget>

VoiceMessageWidget::VoiceMessageWidget(QWidget* history, QString audioFilePath, QString username, uint64_t userId, uint64_t msgID,
                                       qint64 utc, uint16_t duration, const Style::MessageWidget& st)
    : MessageWidget(history, QString(""), userId, msgID, utc, username, st), _audioFilePath(std::move(audioFilePath)), _duration(duration)
{
    setContentsMargins(QMargins(_st.radius, _st.radius, _st.radius, _st.radius));
    setMinimumHeight(_st.fontname->height + _st.radius * 2);

    _audioButton = std::make_unique<AudioButton>(_audioFilePath, this);

    _audioButton->move(_st.radius * 2, _st.fontname->height + _st.radius * 4);
    _audioButton->show();
}

int VoiceMessageWidget::expectedHeight() const { return _st.radius * 7 + _st.fontname->height + _audioButton->height(); }

void VoiceMessageWidget::paintEvent(QPaintEvent* e)
{
    MessageWidget::paintEvent(e);
}

void VoiceMessageWidget::resizeEvent(QResizeEvent* e)
{
    emit geometryChanged(e->size().height() - e->oldSize().height());

    _audioButton->resize(width() - _st.radius * 4 - 1, _audioButton->height());

    MessageWidget::resizeEvent(e);
}
