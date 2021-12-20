#pragma once

#ifdef _WIN32
#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif
#endif

#define _WIN32_WINNT 0x0601

#include <asio.hpp>
#include <chrono>
#include <iostream>
#include <functional>

#include "Message.hpp"
#include "SafeQueue.hpp"
#include "Utility/Utility.hpp"
#include "Utility/WarningSuppression.hpp"
#include "CompressionHandler.hpp"
#include "Handler.hpp"
#include "EncryptionHandler.hpp"
#include "SerializationHandler.hpp"
#include "YasSerializer.hpp"

namespace Network
{
class Connection : public std::enable_shared_from_this<Connection>
{
public:
    /** @enum OwnerType
     *  @brief A connection is "owned" by either a server or a client. /
        And its behaviour is slightly different bewteen the two.
     */
    enum class OwnerType
    {
        SERVER,  /// owner is server
        CLIENT   /// owner is client
    };

private:
    /// Connection "owner"
    OwnerType mOwner = OwnerType::SERVER;
    /// Connection id
    std::uint64_t mConnectionID = uint64_t();

    /// Unique socket to remote
    asio::ip::tcp::socket mSocket;
    /// Overall context for the whole asio instance
    asio::io_context& mContextLink;

    /// References to the incoming queue of the parent object
    SafeQueue<Message>& mIncomingMessagesQueueLink;
    /// Queue all messages to be sent to the remote side of this connection
    SafeQueue<Message> mOutcomingMessagesQueue;

    /// Buffer to store the part of incoming message while it is read
    Message mMessageBuffer;

    void writeHeader()
    {
        yas::shared_buffer bodyBuffer;

        SerializationHandler handler;
        handler.setNext(new CompressionHandler())->setNext(new EncryptionHandler());
        MessageProcessingState result = handler.handleOutcomingMessage(mOutcomingMessagesQueue.front(), bodyBuffer);
        
        Network::Message::MessageHeader outcomingMessageHeader =
            mOutcomingMessagesQueue.front().mHeader;
        outcomingMessageHeader.mBodySize = static_cast<uint32_t>(bodyBuffer.size);
        
        if (result == MessageProcessingState::SUCCESS)
        {
            const auto writeHeaderHandler = [this, bodyBuffer](std::error_code error) {
                if (!error)
                {
                    if (mOutcomingMessagesQueue.front().mBody.has_value())
                    {
                        writeBody(bodyBuffer);
                    }
                    else
                    {
                        mOutcomingMessagesQueue.pop_front();

                        if (!mOutcomingMessagesQueue.empty())
                        {
                            writeHeader();
                        }
                    }
                }
                else
                {
                    std::cout << "[" << mConnectionID << "] Write Header Fail.\n";
                    mSocket.close();
                }
            };

            asio::async_write(mSocket,
                              asio::buffer(&outcomingMessageHeader, sizeof(Message::MessageHeader)),
                              std::bind(writeHeaderHandler, std::placeholders::_1));
        }
    }

    void writeBody(yas::shared_buffer buffer)
    {
        const auto writeBodyHandler = [this](std::error_code error) {
            if (!error)
            {
                mOutcomingMessagesQueue.pop_front();

                if (!mOutcomingMessagesQueue.empty())
                {
                    writeHeader();
                }
            }
            else
            {
                std::cout << "[" << mConnectionID << "] Write Body Fail.\n";
                mSocket.close();
            }
        };

        asio::async_write(mSocket, asio::buffer(buffer.data.get(), buffer.size),
                          std::bind(writeBodyHandler, std::placeholders::_1));
    }

    void readHeader()
    {
        const auto readHeaderHandler = [this](std::error_code error) {
            if (!error)
            {
                if (mMessageBuffer.mHeader.mBodySize > 0)
                {
                    readBody(mMessageBuffer.mHeader.mBodySize);
                }
                else
                {
                    addToIncomingMessageQueue();
                }
            }
            else
            {
                std::cout << "[" << mConnectionID << "] Read Header Fail.\n";
                mSocket.close();
            }
        };

        asio::async_read(mSocket,
                         asio::buffer(&mMessageBuffer.mHeader, sizeof(Message::MessageHeader)),
                         std::bind(readHeaderHandler, std::placeholders::_1));
    }

    void readBody(size_t bodySize)
    {
        yas::shared_buffer buffer;
        buffer.resize(bodySize);

        const auto readBodyHandler = [this, buffer](std::error_code error) {
            if (!error)
            {
                EncryptionHandler handler;
                handler.setNext(new CompressionHandler())->setNext(new SerializationHandler());
                MessageProcessingState result =
                    handler.handleIncomingMessageBody(buffer, mMessageBuffer);

                if (result == MessageProcessingState::SUCCESS)
                {
                    addToIncomingMessageQueue();
                }
            }
            else
            {
                std::cout << "[" << mConnectionID << "] Read Body Fail.\n";
                mSocket.close();
            }
        };

        asio::async_read(mSocket, asio::buffer(buffer.data.get(), buffer.size),
                         std::bind(readBodyHandler, std::placeholders::_1));
    }

    void addToIncomingMessageQueue()
    {
        if (mOwner == OwnerType::SERVER)
        {
            mMessageBuffer.mRemote = this->shared_from_this();
            mIncomingMessagesQueueLink.push_back(mMessageBuffer);
        }
        else
        {
            mIncomingMessagesQueueLink.push_back(mMessageBuffer);
        }

        readHeader();
    }

public:
    Connection(const OwnerType& owner, asio::io_context& contextLink, asio::ip::tcp::socket socket,
               SafeQueue<Message>& incomingMessagesQueueLink)
        : mOwner(owner),
          mSocket(std::move(socket)),
          mContextLink(contextLink),
          mIncomingMessagesQueueLink(incomingMessagesQueueLink)
    {
    }

    /**
     * @brief Method getting connection id.
     * @details This id is used system wide - it is how clients will understand other clients /
     * whole system.
     * @return mId - connection id.
     */
    std::uint64_t getID() const { return mConnectionID; }

    void connectToClient(const uint64_t uid = uint64_t())
    {
        if (mOwner == OwnerType::SERVER)
        {
            if (mSocket.is_open())
            {
                mConnectionID = uid;
                readHeader();
            }
        }
    }

    // clang-format off
    suppressWarning(4100, "-Wunused-parameter")
    void connectToServer(const asio::ip::tcp::resolver::results_type& endpoint)
    {
        if (mOwner == OwnerType::CLIENT)
        {
            asio::async_connect(mSocket, endpoint,
                                [this](std::error_code ec, asio::ip::tcp::endpoint endpoint) {
                                    if (!ec)
                                    {
                                        readHeader();
                                    }
                                });
        }
    }
    restoreWarning

    void disconnect()
    {
        if (isConnected())
        {
            asio::post(mContextLink, [this]() { mSocket.close(); });
        }
    }

    bool isConnected() const { return mSocket.is_open(); }

    void send(const Message& message)
    {
        asio::post(mContextLink, [this, message]() {
            bool isMessageExist = !mOutcomingMessagesQueue.empty();

            mOutcomingMessagesQueue.push_back(message);

            if (!isMessageExist)
            {
                writeHeader();
            }
        });
    }
};
}  // namespace Network
