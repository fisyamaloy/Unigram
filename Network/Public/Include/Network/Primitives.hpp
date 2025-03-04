#pragma once

#include <Utility/Utility.hpp>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <map>
#include <string>
#include <variant>

namespace Network
{
/**
 * @brief The ChannelInfo struct
 * @details Channel Info struct for information about channels
 *          contain creator ID, channel ID and channel Name.
 */
struct ChannelInfo
{
    /// creator ID uint64_t variable
    std::uint64_t creatorID;
    /// channel ID uint64_t variable
    std::uint64_t channelID;
    /// channel name string variable
    std::string channelName;

public:
    /// Default ChannelInfo constructor
    ChannelInfo() = default;

    /// ChannelInfo constructor with initializing list
    ChannelInfo(const std::uint64_t creatorID, const std::uint64_t channelID, const std::string& channelName)
        : creatorID(creatorID), channelID(channelID), channelName(channelName)
    {
    }
    /// Default ChannelInfo destructor
    ~ChannelInfo() = default;

    /// Operator == to compare channels info
    friend bool operator==(const ChannelInfo& first, const ChannelInfo& second)
    {
        return first.channelID == second.channelID && first.creatorID == second.creatorID && first.channelName == second.channelName;
    }
};

/// Serialize method which serialize each field
template <typename Archive>
void serialize(Archive& ar, Network::ChannelInfo& o)
{
    ar & o.creatorID & o.channelID & o.channelName;
}

using ChannelDeleteInfo = ChannelInfo;
using ChannelLeaveInfo  = ChannelInfo;

/**
 * @brief The ChannelSubscriptionInfo struct
 * @details Channel Info struct for information about the channel that the user subscribed.
 *          contain channel ID.
 */
struct ChannelSubscriptionInfo
{
    /// channel ID uint64_t variable
    std::uint64_t channelID;
    /// user ID uint64_t variable
    std::uint64_t userID;

public:
    /// Default ChannelInfo constructor
    ChannelSubscriptionInfo() = default;

    /// ChannelInfo constructor with initializing list
    ChannelSubscriptionInfo(const std::uint64_t channelID) : channelID(channelID) {}
    /// Default ChannelSubscriptionInfo destructor
    ~ChannelSubscriptionInfo() = default;

    /// Operator == to compare channel info
    friend bool operator==(const ChannelSubscriptionInfo& first, const ChannelSubscriptionInfo& second)
    {
        return first.channelID == second.channelID && first.userID == second.userID;
    }
};

/**
 * @brief helper function for serializing
 * @ref ChannelSubscriptionInfo structure
 */
template <typename Archive>
void serialize(Archive& ar, Network::ChannelSubscriptionInfo& o)
{
    ar & o.channelID;
}

/**
 * @brief Struct for storing and transmitting login information
 */
struct LoginInfo
{
    /// user's login as string variable
    std::string login;
    /// pwdHash hash of user's password as string variable
    std::string pwdHash;
    /// Default LoginInfo constructor
    LoginInfo() = default;
    /// Default LoginInfo copy constructor
    LoginInfo(const LoginInfo&) = default;
    /// LoginInfo constructor with initializing list
    explicit LoginInfo(const std::string& login, const std::string& passwordHash) : login(login), pwdHash(passwordHash) {}
    /// Default LoginInfo destructor
    ~LoginInfo() = default;
};

/**
 * @brief helper function for serializing @ref LoginInfo structure
 */
template <typename Archive>
void serialize(Archive& ar, LoginInfo& o)
{
    ar & o.login & o.pwdHash;
}

/**
 * @brief The RegistrationInfo struct
 * @details Info about email, login and passwordHash.
 */
struct RegistrationInfo
{
    /// email string variable
    std::string email;
    /// login string variable
    std::string login;
    /// passwordHash string variable
    std::string passwordHash;
    /// Default RegistrationInfo constructor
    RegistrationInfo() = default;

    /// RegistrationInfo constructor with initializing list
    explicit RegistrationInfo(const std::string& email, const std::string& login, const std::string& passwordHash)
        : email(email), login(login), passwordHash(passwordHash)
    {
    }

    /// Default RegistrationInfo copy constructor
    RegistrationInfo(const RegistrationInfo&) = default;
    /// Default RegistrationInfo destructor
    ~RegistrationInfo() = default;

    /// Default Registration copy assignment constructor
    RegistrationInfo& operator=(const RegistrationInfo& other) = default;

    /// Operator == to compare registration info
    friend bool operator==(const RegistrationInfo& registrationInfo1, const RegistrationInfo& registrationInfo2)
    {
        return registrationInfo1.email == registrationInfo2.email && registrationInfo1.login == registrationInfo2.login &&
               registrationInfo1.passwordHash == registrationInfo2.passwordHash;
    }
};

/// Serialize method for serialize registration info for each field
template <typename Archive>
void serialize(Archive& ar, Network::RegistrationInfo& o)
{
    ar & o.email & o.login & o.passwordHash;
}

enum class MessageInfoType : std::uint8_t
{
    TEXT  = 'T',
    AUDIO = 'A',
    VIDEO = 'V',
};

struct TextMessage
{
    std::string text;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & text;
    }
};

struct VoiceMessage
{
    std::string          fileName;
    std::uint16_t        durationSeconds;
    std::vector<uint8_t> data = {};

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & fileName & durationSeconds & data;
    }
};

struct VideoMessage
{
    std::string          fileName;
    std::uint16_t        durationSeconds;
    std::vector<uint8_t> data;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & fileName & durationSeconds & data;
    }
};

/**
 * @brief The MessageInfo struct
 * @details Message Info contains information about user message and his type.
 */
struct MessageInfo
{
    /// channel ID uint64_t variable
    std::uint64_t channelID;
    /// msg ID uint64_t variable
    std::uint64_t msgID;
    /// sender ID uint64_t variable
    std::uint64_t senderID;
    /// user Login string variable
    std::string userLogin;
    /// recipient ID uint64_t variable
    std::uint64_t recipientID;
    /// time in milliseconds since Epoch (1970-01-01 00:00:00 UTC)
    std::string time = Utility::getTimeNow();
    /// reactions (reaction_id, reaction_count)
    std::map<std::uint32_t, std::uint32_t> reactions = {};

    std::variant<TextMessage, VoiceMessage, VideoMessage> content;

    MessageInfoType type = MessageInfoType::TEXT;

    /// Default MessageIndo constructor
    MessageInfo() = default;
    /// MessageIndo constructor with initializing list
    MessageInfo(const uint64_t channelID, std::string text)
    {
        this->channelID = channelID;
        this->content   = TextMessage{std::move(text)};
        this->type      = MessageInfoType::TEXT;
    }

    MessageInfo(const std::uint64_t channelID, std::string fileName, std::vector<std::uint8_t> data, const std::uint16_t duration,
                MessageInfoType type)
    {
        this->channelID = channelID;
        this->type      = type;

        if (type == MessageInfoType::AUDIO)
        {
            this->content = VoiceMessage{std::move(fileName), duration, std::move(data)};
        }
        else if (type == MessageInfoType::VIDEO)
        {
            this->content = VideoMessage{std::move(fileName), duration, std::move(data)};
        }
    }

    /// Default MessageIndo copy constructor
    MessageInfo(const MessageInfo&) = default;
    /// Default MessageIndo destructor
    ~MessageInfo() = default;

    template <typename T>
    const T& getContent() const
    {
        assert(std::holds_alternative<T>(content) && "Invalid type access in getContent");
        return std::get<T>(content);
    }

    template <typename T>
    void setContent(T newContent)
    {
        if constexpr (std::is_same_v<T, std::string>)
        {
            content = TextMessage{newContent};
            type    = MessageInfoType::TEXT;
        }
        else if constexpr (std::is_same_v<T, VoiceMessage>)
        {
            content = std::move(newContent);
            type    = MessageInfoType::AUDIO;
        }
        else if constexpr (std::is_same_v<T, VideoMessage>)
        {
            content = std::move(newContent);
            type    = MessageInfoType::VIDEO;
        }
    }

    /// Operator == to compare Message Info
    friend bool operator==(const MessageInfo& first, const MessageInfo& second)
    {
        return first.channelID == second.channelID && first.time == second.time && first.msgID == second.msgID &&
               first.userLogin == second.userLogin;
    }

    /// Operator < to compare MessageInfo
    friend bool operator<(const MessageInfo& lhs, const MessageInfo& rhs) { return lhs.time < rhs.time; }
};

/**
 * @brief The ReplyInfo struct
 * @details Reply Info contains channel ID, message, message ID,
 *          sender ID, msgIdOwner and sender ID.
 */
struct ReplyInfo
{
    /// channel ID uint64_t variable
    std::uint64_t channelID;
    /// message string variable
    std::string message;
    /// msg ID uint64_t variable
    std::uint64_t msgID;
    /// msg ID owner uint64_t variable
    std::uint64_t msgIdOwner;
    /// sender ID uint64_t variable
    std::uint64_t senderID;
    /// user Login string variable
    std::string userLogin;
    /// Default ReplyInfo constructor
    ReplyInfo() = default;

    /// ReplyInfo constructor with initializing list
    explicit ReplyInfo(const std::uint64_t& channelID, const std::string& text) : channelID(channelID), message(text) {}
    /// Default Reply copy constructor
    ReplyInfo(const ReplyInfo&) = default;
    /// Default ReplyInfo destructor
    ~ReplyInfo() = default;

    /// Operator == to compare Reply Info
    friend bool operator==(const ReplyInfo& first, const ReplyInfo& second)
    {
        return first.message == second.message && first.msgID == second.msgID && first.senderID == second.senderID &&
               first.msgIdOwner == second.msgIdOwner && first.channelID == second.channelID && first.userLogin == second.userLogin;
    }
};

/// Serialize method for serialize ReplyInfo for each field
template <typename Archive>
void serialize(Archive& ar, Network::ReplyInfo& o)
{
    ar & o.channelID & o.message & o.msgID & o.senderID & o.msgIdOwner & o.userLogin;
}

}  // namespace Network
