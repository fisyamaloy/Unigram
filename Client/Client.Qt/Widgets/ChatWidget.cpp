#include "ChatWidget.hpp"

#include <QtEvents>
#include <utility>

#include "Application.hpp"
#include "ChatHistory.hpp"

ChatWidget::ChatWidget(QWidget* parent) : QWidget(parent)
{
    setContentsMargins(0, 0, 0, 0);

    _channelBar   = std::make_unique<ChannelBar>(this);
    _chatHistory  = std::make_unique<ChatHistory>(this);
    _textEdit     = std::make_unique<TextEdit>(this);
    _replyWidget  = std::make_unique<ReplyWidget>(this);
    _requestTimer = std::make_unique<QTimer>();

    connect(_requestTimer.get(), &QTimer::timeout, this, &ChatWidget::requestMessages);

    /// Once in a second
    _requestTimer->start(1000);
    _replyWidget->hide();

    setMinimumWidth(st::chatWidgetMinWidth);
    connect(_chatHistory.get(), &ChatHistory::createReplySignal, this, &ChatWidget::setReply);
    connect(_textEdit.get(), &TextEdit::sendMessage, this, &ChatWidget::newMessage);

    connect(_replyWidget.get(), &ReplyWidget::visibilityChanged, [=](bool) { updateLayout(); });
    connect(_textEdit.get(), &TextEdit::textChanged, [=]() { updateLayout(); });

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
