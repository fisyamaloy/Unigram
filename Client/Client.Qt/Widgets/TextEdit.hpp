#pragma once
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTimer>
#include <memory>

#include "Buttons.hpp"
#include "InputFields.hpp"
#include "Label.hpp"
#include "Settings.hpp"

enum class VoiceRecordButtonStatus : std::uint8_t
{
    STOP = 0,
    START
};

/**
 *  @class TextEdit
 *  @brief Widget with a text edit field and an edit bar.\
 *  The edit bar has a Bold, Italics and Underscores which makes selected text, and the send button.
 */
class TextEdit : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief Constructor for widget textEdit.
     * @param parent Parent widget
     */
    explicit TextEdit(QWidget* parent = nullptr);
    /**
     * @brief Method for getting text from the text edit field.
     * @return text edit field content.
     */
    [[nodiscard]] QString getText() const;

    /// Expected height of Text Edit
    int expectedHeight();

    /// clear text
    void clear();
    /// Destructor for text edit
    ~TextEdit() override;

public slots:
    /// send message
    void sendButtonClick();
    void recordingVoiceButtonClick();

signals:
    /// message send
    void sendMessage(QString textMessage);
    /// text changed
    void textChanged();
    /// audio button clicked
    void startVoiceRecord();
    void stopVoiceRecord(const int duration);

private slots:
    void styleButtonClick(const QString& symbolStart, const QString& symbolEnd);
    void updateRecordingTime();

private:
    void delSymbolsInSelection(QString& text, int& start, int& end, int symbolSize);
    void delSymbolsOutSelection(QString& text, int& start, int& end, int symbolSize);
    void insertSymbolsInSelection(QTextCursor& cursor, int& start, int& end, int symbolSize, const QString& symbolStart,
                                  const QString& symbolEnd);
    void selectText(QTextCursor& cursor, int start, int end);

    void finalizeVoiceRecording();
    void initializeVoiceRecording();

private:
    Settings& _settings;

    const int               _symbolSize           = 3;
    const QString           _boldSymbolOpen       = "<B>";
    const QString           _boldSymbolClose      = "</B>";
    const QString           _italicSymbolOpen     = "<I>";
    const QString           _italicSymbolClose    = "</I>";
    const QString           _underlineSymbolOpen  = "<U>";
    const QString           _underlineSymbolClose = "</U>";
    VoiceRecordButtonStatus _voiceButtonStatus    = VoiceRecordButtonStatus::STOP;

    std::unique_ptr<QVBoxLayout>  _mainVerticalLayout;
    std::unique_ptr<QHBoxLayout>  _horizontalButtonLayout;
    std::unique_ptr<FlatButton>   _boldnessButton;
    std::unique_ptr<FlatButton>   _italicButton;
    std::unique_ptr<FlatButton>   _underlineButton;
    std::unique_ptr<Label>        _recordingSecondsLabel;
    std::unique_ptr<IconButton>   _recordingVoiceButton;
    std::unique_ptr<FlatButton>   _sendButton;
    std::unique_ptr<FlatTextEdit> _messageInput;
    std::unique_ptr<QSpacerItem>  _horizontalButtonSpacer;
    std::unique_ptr<QTimer>       _recordTimer;
    int                           _milliseconds;
    int                           _seconds;

    static constexpr int MAX_RECORDING_TIME = 60;
};
