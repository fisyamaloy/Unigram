#pragma once
#include "Buttons.hpp"
#include "Widgets/MessageWidget.hpp"

class VoiceMessageWidget : public MessageWidget
{
    Q_OBJECT

public:
    VoiceMessageWidget(QWidget* history, QString audioFilePath, QString username, uint64_t userId, uint64_t msgID, qint64 utc,
                       uint16_t duration, const Style::MessageWidget& st = st::defaultMessageWidget);

public:
    int expectedHeight() const override;

    friend bool operator<(const VoiceMessageWidget& lhs, const VoiceMessageWidget& rhs) { return lhs._datetime < rhs._datetime; }

signals:
    /// Signal for changing reply message geometry
    void geometryChanged(int);

protected:
    void paintEvent(QPaintEvent* e) override;
    void resizeEvent(QResizeEvent* e) override;

private:
    QString  _audioFilePath;
    uint16_t _duration;

    std::unique_ptr<AudioButton> _audioButton;
};
