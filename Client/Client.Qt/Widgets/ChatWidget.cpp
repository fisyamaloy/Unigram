#include "ChatWidget.hpp"

#include <QFile>
#include <QtCore>
#include <QtEvents>
#include <fstream>
#include <utility>

#include "Application.hpp"
#include "ChatHistory.hpp"
// #include "qstandardpaths.h"

static constexpr int SAMPLE_RATE   = 44100;
static constexpr int CHANNEL_COUNT = 1;

VoiceRecorder::VoiceRecorder(QObject* parent) : QObject(parent)
{
    _format.setSampleRate(SAMPLE_RATE);
    _format.setChannelCount(CHANNEL_COUNT);
    _format.setSampleFormat(QAudioFormat::Int16);
}

VoiceRecorder::~VoiceRecorder() { stopRecording(); }

void VoiceRecorder::startRecording()
{
    const QString timestamp   = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    const QString mp3FilePath = QString("voice_%1.mp3").arg(timestamp);
    _mp3File.setFileName(mp3FilePath);

    if (!_mp3File.open(QIODevice::WriteOnly))
    {
        qDebug() << "ERROR: unable to create mp3 file";
        return;
    }

    _pLame = lame_init();
    lame_set_mode(_pLame, MONO);
    lame_set_in_samplerate(_pLame, SAMPLE_RATE);
    lame_set_num_channels(_pLame, CHANNEL_COUNT);
    lame_set_quality(_pLame, 5);
    lame_set_VBR(_pLame, vbr_default);
    lame_set_bWriteVbrTag(_pLame, 1);
    lame_init_params(_pLame);

    _audioStream.reset();
    _audioSource.reset();

    _audioSource = std::make_unique<QAudioSource>(_format, this);
    _audioStream.reset(_audioSource->start());
    connect(_audioStream.get(), &QIODevice::readyRead, this, &VoiceRecorder::writeAudioData);
    qDebug() << "Start recording";
}

void VoiceRecorder::stopRecording()
{
    if (_audioSource)
    {
        _audioSource->stop();
    }
    if (_audioStream)
    {
        _audioStream.reset();
    }

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
    const int  numSamples = static_cast<int>(pcmData.size()) / (CHANNEL_COUNT * sizeof(int16_t));

    int mp3BufferSize = 1.25 * numSamples + 7200;

    unsigned char* mp3Buffer = new unsigned char[mp3BufferSize];
    int            mp3Size   = lame_encode_buffer(_pLame, pcmBuffer, nullptr, numSamples, mp3Buffer, mp3BufferSize);
    if (mp3Size > 0)
    {
        _mp3File.write((char*)mp3Buffer, mp3Size);
    }

    delete[] mp3Buffer;
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
    connect(_textEdit.get(), &TextEdit::startVoiceRecord, this, &ChatWidget::recordVoiceMessage);
    connect(_textEdit.get(), &TextEdit::stopVoiceRecord, this, &ChatWidget::stopRecordingVoiceMessage);

    connect(_replyWidget.get(), &ReplyWidget::visibilityChanged, [this](bool) { updateLayout(); });
    connect(_textEdit.get(), &TextEdit::textChanged, [this]() { updateLayout(); });

    connect(ReceiverManager::instance(), &ReceiverManager::onReplyHistoryAnswer, this, &ChatWidget::addReplies);
    connect(ReceiverManager::instance(), &ReceiverManager::onMessageHistoryAnswer, this, &ChatWidget::addMessages);
}

void ChatWidget::newMessage(const QString& messageText)
{
    oApp->connectionManager()->storeTextMessage(messageText.toStdString(), _channelID);
    if (!_replyWidget->isHidden())
    {
        oApp->connectionManager()->storeReply(_replyWidget->getMessage().toStdString(), _channelID, _replyWidget->getMessageId());
        _replyWidget->close();
    }
}

static void saveFile(const std::string& fileName, const std::vector<std::uint8_t>& fileRawData)
{
    std::ofstream out(fileName, std::ios_base::binary);
    if (out)
    {
        out.write(reinterpret_cast<const char*>(fileRawData.data()), fileRawData.size());
        out.close();
    }
}

void ChatWidget::addMessages(const std::vector<Network::MessageInfo>& messages)
{
    if (messages.empty()) return;
    if (messages.front().channelID != _channelID) return;  // It could be messages for other channel

    for (const auto& message : messages)
    {
        if (message.type == Network::MessageInfoType::AUDIO)
        {
            auto& voiceMsg = message.getContent<Network::VoiceMessage>();
            saveFile(voiceMsg.fileName, voiceMsg.data);
        }
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
}

void ChatWidget::stopRecordingVoiceMessage(const int duration)
{
    qDebug() << "Stop recording\n";
    _voiceRecorder->stopRecording();
    const QFileInfo recorderedFileInfo = _voiceRecorder->getMP3FileInfo();
    qDebug() << "record path: " << recorderedFileInfo.absoluteFilePath();

    oApp->connectionManager()->storeVoiceMessage(recorderedFileInfo.filesystemAbsoluteFilePath(), _channelID,
                                                 static_cast<uint16_t>(duration));
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
