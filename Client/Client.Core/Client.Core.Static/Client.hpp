#pragma once

#include "Network/Connection.hpp"
#include "Network/Message.hpp"
#include "Network/SafeQueue.hpp"

namespace Network
{
class Client
{
public:
    ~Client();

    bool connectToServer(const std::string_view& host, const uint16_t port);
    void disconnectFromServer();

    [[nodiscard]] bool isConnected() const;

    void send(const Message& message) const;

    SafeQueue<Message>& incoming() { return _incomingMessagesQueue; }

    void loop();

    void pingServer() const;

    void messageAll() const;
   
    void userRegistration(const std::string& email, const std::string& login, const std::string& password) const;

    void userAuthorization(const std::string& login, const std::string& password) const;

protected:
    virtual void onDisconnect();
    virtual void onMessageSendFailed(const Message& message) const;

    virtual void onServerAccepted();
    virtual void onServerPing(double timestamp);
    virtual void onRegistrationAnswer(Utility::RegistrationCodes registrationCode);
    virtual void onLoginAnswer(bool success);

private:
    asio::io_context _context;
    std::thread      _contextThread;

    bool _serverAccept = false;

    std::unique_ptr<Connection> _connection;

    SafeQueue<Message> _incomingMessagesQueue;
};
}  // namespace Network
