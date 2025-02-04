#pragma once
#include <Network/Primitives.hpp>
#include <QAudioSource>
#include <QFile>

#include "Widgets/ChannelBar.hpp"
#include "Widgets/InputFields.hpp"
#include "Widgets/MessageWidget.hpp"
#include "Widgets/TextEdit.hpp"
#include "lame/lame.h"

struct QAudioDeviceDeleter
{
    void operator()(QIODevice* device) const
    {
        if (device)
        {
            device->close();
        }
    }
};

class VoiceRecorder : public QObject
{
    Q_OBJECT

public:
    VoiceRecorder(QObject* parent = nullptr);
    VoiceRecorder(const VoiceRecorder&) = delete;
    ~VoiceRecorder();

    void startRecording();
    void stopRecording();

    QString getMP3FileName() const { return _mp3FileName; }
    QString getPCMFileName() const { return _pcmFileName; }

private:
    void writeAudioData();
    void convertToMp3();

private:
    std::unique_ptr<QAudioSource>                   _audioSource;
    std::unique_ptr<QIODevice, QAudioDeviceDeleter> _audioStream;

    QFile   _mp3File;
    QString _pcmFileName;
    QString _mp3FileName;
    lame_t  _pLame;
};

/**
 *  @class ChatWidget
 *  @brief chatWidget stores and displays messages and replies.
 */
class ChatWidget : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief Constructor for chat widget
     * @param parent Parent widget
     */
    explicit ChatWidget(QWidget* parent = nullptr);
    /**
     * @brief Method for chat widget
     * @param ID of channel. It's format uint64_t
     */
    void setChannelID(const std::uint64_t channelID) { _channelID = channelID; }

protected:
    /// Handle resize
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void newMessage(const QString& messageText);
    void setReply(QString messageText, QString username, uint64_t messageId);
    void addMessages(const std::vector<Network::MessageInfo>& messages);
    void addReplies(const std::vector<Network::ReplyInfo>& replies);
    void recordVoiceMessage();
    void stopRecordingVoiceMessage();

    void requestMessages() const;
    void updateLayout();

private:
    std::unique_ptr<ChannelBar>    _channelBar;
    std::unique_ptr<ReplyWidget>   _replyWidget;
    std::unique_ptr<ChatHistory>   _chatHistory;
    std::unique_ptr<TextEdit>      _textEdit;
    std::unique_ptr<VoiceRecorder> _voiceRecorder;

    std::unique_ptr<QTimer> _requestTimer;
    std::uint64_t           _channelID{};
};
