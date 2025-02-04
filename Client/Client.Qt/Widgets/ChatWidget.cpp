﻿#include "ChatWidget.hpp"

#include <QFile>
#include <QtEvents>
#include <utility>

#include "Application.hpp"
#include "ChatHistory.hpp"
// #include "qstandardpaths.h"

static constexpr int BIT_RATE      = 44100;
static constexpr int CHANNEL_COUNT = 1;

VoiceRecorder::VoiceRecorder(QObject* parent) : QObject(parent)
{
    QAudioFormat _format;
    _format.setSampleRate(BIT_RATE);
    _format.setChannelCount(CHANNEL_COUNT);
    _format.setSampleFormat(QAudioFormat::Int16);

    _audioSource = std::make_unique<QAudioSource>(_format, this);
    // outputFile.setFileName(QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/voice_message.pcm");
}

VoiceRecorder::~VoiceRecorder() { stopRecording(); }

void VoiceRecorder::startRecording()
{
    const QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    _mp3FileName            = QString("voice_%1.mp3").arg(timestamp);
    _mp3File.setFileName(_mp3FileName);

    if (!_mp3File.open(QIODevice::WriteOnly))
    {
        qDebug() << "ERROR: unable to create mp3 file";
        return;
    }

    _pLame = lame_init();
    lame_set_mode(_pLame, MONO);
    lame_set_in_samplerate(_pLame, BIT_RATE);
    lame_set_num_channels(_pLame, CHANNEL_COUNT);
    lame_set_quality(_pLame, 2);
    lame_set_VBR(_pLame, vbr_default);
    lame_init_params(_pLame);

    _audioStream.reset(_audioSource->start());
    connect(_audioStream.get(), &QIODevice::readyRead, this, &VoiceRecorder::writeAudioData);
    qDebug() << "Start recording";
}

void VoiceRecorder::stopRecording()
{
    if (!_audioSource)
    {
        return;
    }

    _audioSource->stop();

    if (_pLame)
    {
        unsigned char mp3Buffer[1024];
        int           write = lame_encode_flush(_pLame, mp3Buffer, sizeof(mp3Buffer));
        _mp3File.write((char*)mp3Buffer, write);
        lame_close(_pLame);
        _pLame = nullptr;
    }

    _mp3File.close();
    qDebug() << "Recording stopped. Convertation to mp3...";
}

void VoiceRecorder::writeAudioData()
{
    QByteArray pcmData    = _audioStream->readAll();
    short int* pcmBuffer  = reinterpret_cast<short int*>(pcmData.data());
    int        numSamples = static_cast<int>(pcmData.size()) / sizeof(short int);

    unsigned char mp3Buffer[8192];
    int           mp3Size = lame_encode_buffer(_pLame, pcmBuffer, nullptr, numSamples, mp3Buffer, sizeof(mp3Buffer));

    if (mp3Size > 0)
    {
        _mp3File.write((char*)mp3Buffer, mp3Size);
    }
}

ChatWidget::ChatWidget(QWidget* parent) : QWidget(parent)
{
    setContentsMargins(0, 0, 0, 0);

    _channelBar    = std::make_unique<ChannelBar>(this);
    _chatHistory   = std::make_unique<ChatHistory>(this);
    _textEdit      = std::make_unique<TextEdit>(this);
    _replyWidget   = std::make_unique<ReplyWidget>(this);
    _requestTimer  = std::make_unique<QTimer>();
    _voiceRecorder = std::make_unique<VoiceRecorder>(this);

    connect(_requestTimer.get(), &QTimer::timeout, this, &ChatWidget::requestMessages);

    /// Once in a second
    _requestTimer->start(1000);
    _replyWidget->hide();

    setMinimumWidth(st::chatWidgetMinWidth);
    connect(_chatHistory.get(), &ChatHistory::createReplySignal, this, &ChatWidget::setReply);
    connect(_textEdit.get(), &TextEdit::sendMessage, this, &ChatWidget::newMessage);
    connect(_textEdit.get(), &TextEdit::startAudioRecord, this, &ChatWidget::recordVoiceMessage);
    connect(_textEdit.get(), &TextEdit::stopAudioRecord, this, &ChatWidget::stopRecordingVoiceMessage);

    connect(_replyWidget.get(), &ReplyWidget::visibilityChanged, [this](bool) { updateLayout(); });
    connect(_textEdit.get(), &TextEdit::textChanged, [this]() { updateLayout(); });

    connect(ReceiverManager::instance(), &ReceiverManager::onReplyHistoryAnswer, this, &ChatWidget::addReplies);
    connect(ReceiverManager::instance(), &ReceiverManager::onMessageHistoryAnswer, this, &ChatWidget::addMessages);
}

void ChatWidget::newMessage(const QString& messageText)
{
    oApp->connectionManager()->storeMessage(messageText.toStdString(), _channelID);
    if (!_replyWidget->isHidden())
    {
        oApp->connectionManager()->storeReply(_replyWidget->getMessage().toStdString(), _channelID, _replyWidget->getMessageId());
        _replyWidget->close();
    }
}

void ChatWidget::addMessages(const std::vector<Network::MessageInfo>& messages)
{
    if (messages.empty()) return;
    if (messages.front().channelID != _channelID) return;  // It could be messages for other channel

    for (const auto& message : messages)
    {
        _chatHistory->addMessage(message);
    }
}

void ChatWidget::addReplies(const std::vector<Network::ReplyInfo>& replies)
{
    if (replies.empty()) return;
    if (replies.front().channelID != _channelID) return;  // It could be replies for other channel

    for (const auto& reply : replies)
    {
        _chatHistory->addReply(reply);
    }
}

void ChatWidget::recordVoiceMessage()
{
    qDebug() << "Start recording";
    _voiceRecorder->startRecording();
    // oApp->connectionManager()->sendAudioMessage([aduio_data], _channelID);
}

void ChatWidget::stopRecordingVoiceMessage()
{
    qDebug() << "Stop recording\n";
    _voiceRecorder->stopRecording();
    const QString recorderedFileName = _voiceRecorder->getMP3FileName();
    qDebug() << "record name: " << recorderedFileName;
}

void ChatWidget::requestMessages() const
{
    if (oApp->connectionManager()->isConnected())
    {
        oApp->connectionManager()->askForMessageHistory(_channelID);
        oApp->connectionManager()->askForReplyHistory(_channelID);
    }
}

void ChatWidget::setReply(QString messageText, QString username, uint64_t messageId)
{
    _replyWidget->setReply(std::move(messageText), std::move(username), messageId);
    _replyWidget->show();
}

void ChatWidget::updateLayout()
{
    const auto& size = this->size();
    _replyWidget->setFixedWidth(size.width());  // Reply on whole width
    _channelBar->setFixedWidth(size.width());

    auto textEditExpectedHeightBefore = _textEdit->expectedHeight();  // Text edit size before resize
    _textEdit->resize(size.width(), textEditExpectedHeightBefore);
    auto textEditExpectedHeightAfter = _textEdit->expectedHeight();   // Text edit size after resize
    if (textEditExpectedHeightBefore != textEditExpectedHeightAfter)  // Need to resize again
        _textEdit->resize(size.width(), textEditExpectedHeightAfter);

    _chatHistory->resize(size.width(), size.height() - _textEdit->height() - _channelBar->height() -
                                           (_replyWidget->isHidden() ? 0 : _replyWidget->height()));

    _chatHistory->move(0, _channelBar->height());

    _textEdit->move(0, size.height() - _textEdit->height());
    _replyWidget->move(0, _textEdit->y() - _replyWidget->height());
}

void ChatWidget::resizeEvent(QResizeEvent*) { updateLayout(); }
