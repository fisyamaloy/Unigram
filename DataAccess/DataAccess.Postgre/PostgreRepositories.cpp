#include "PostgreRepositories.hpp"

#include <limits>

namespace DataAccess
{
std::vector<Network::ChannelInfo> ChannelsRepository::getAllChannelsList()
{
    pTable->changeTable("channels");
    auto channelListRow = pTable->Select()->columns({"*"})->execute();

    std::vector<Network::ChannelInfo> result;
    if (channelListRow.has_value())
    {
        Network::ChannelInfo channelInfo;
        for (auto&& value : channelListRow.value())
        {
            channelInfo.channelID   = value[0].as<std::uint64_t>();
            channelInfo.channelName = value[1].as<std::string>();
            channelInfo.creatorID   = value[2].as<std::uint64_t>();
            result.push_back(channelInfo);
        }
    }

    return result;
}

Utility::ChannelLeaveCodes ChannelsRepository::leaveChannel(const Network::ChannelLeaveInfo& channel)
{
    pTable->changeTable("channels");
    auto findIdChannel = pTable->Select()->columns({"id"})->Where("channel_name = '" + channel.channelName + "'")->execute();
    pTable->changeTable("user_channels");
    if (!findIdChannel.has_value())
    {
        return Utility::ChannelLeaveCodes::CHANNEL_NOT_FOUND;
    }
    auto findChannel = pTable->Select()
                           ->columns({"*"})
                           ->Where("user_id = " + std::to_string(channel.creatorID) + " AND " +
                                   "channel_id = " + std::to_string(findIdChannel.value()[0][0].as<std::uint64_t>()))
                           ->execute();
    if (findChannel.has_value())
    {
        auto result = pTable->Delete()
                          ->Where("user_id = " + std::to_string(findChannel.value()[0][0].as<std::uint64_t>()) + " AND " +
                                  "channel_id = " + std::to_string(findChannel.value()[0][1].as<std::uint64_t>()))
                          ->execute();
        if (result.has_value())
        {
            return Utility::ChannelLeaveCodes::FAILED;
        }
    }
    else
    {
        return Utility::ChannelLeaveCodes::CHANNEL_NOT_FOUND;
    }
    return Utility::ChannelLeaveCodes::SUCCESS;
}

Utility::ChannelDeleteCode ChannelsRepository::deleteChannel(const Network::ChannelDeleteInfo& channel)
{
    pTable->changeTable("channels");
    auto findChannel = pTable->Select()->columns({"creator_id"})->Where("channel_name = '" + channel.channelName + "'")->execute();
    if (!findChannel.has_value())
    {
        return Utility::ChannelDeleteCode::CHANNEL_NOT_FOUND;
    }
    if (findChannel.value()[0][0].as<uint64_t>() != channel.creatorID)
    {
        return Utility::ChannelDeleteCode::CHANNEL_IS_NOT_USER;
    }
    auto result =
        pTable->Delete()
            ->Where("channel_name = '" + channel.channelName + "'" + " AND " + "creator_id = " + std::to_string(channel.creatorID))
            ->execute();
    if (result.has_value())
    {
        return Utility::ChannelDeleteCode::FAILED;
    }
    return Utility::ChannelDeleteCode::SUCCESS;
}

Utility::ChannelCreateCodes ChannelsRepository::createChannel(const Network::ChannelInfo& channel)
{
    pTable->changeTable("channels");
    auto findChannel = pTable->Select()->columns({"channel_name"})->Where("channel_name = '" + channel.channelName + "'")->execute();
    if (findChannel.has_value())
    {
        if (findChannel.value()[0][0].as<std::string>() == channel.channelName)
        {
            return Utility::ChannelCreateCodes::CHANNEL_ALREADY_CREATED;
        }
    }

    std::tuple channelData{std::pair{"channel_name", channel.channelName}, std::pair{"creator_id", channel.creatorID},
                           std::pair{"user_limit", 1'000'000}};
    auto       result = pTable->Insert()->columns(channelData)->execute();

    if (result.has_value())
    {
        return Utility::ChannelCreateCodes::FAILED;
    }

    auto newChannel   = pTable->Select()->columns({"id"})->Where("channel_name = '" + channel.channelName + "'")->execute();
    auto IDNewChannel = newChannel.value()[0][0].as<uint64_t>();

    std::tuple SubscribNewChannelData{std::pair{"user_id", channel.creatorID}, std::pair{"channel_id", IDNewChannel}};

    pTable->changeTable("user_channels");
    pTable->Insert()->columns(SubscribNewChannelData)->execute();
    return Utility::ChannelCreateCodes::SUCCESS;
}

std::uint64_t LoginRepository::loginUser(const Network::LoginInfo& loginInfo)
{
    try
    {
        pTable->changeTable("users");
        auto queryResult = pTable->Select()->columns({"password_hash", "id"})->Where("login='" + loginInfo.login + "'")->execute().value();

        if (std::string(queryResult[0][0].c_str()) == loginInfo.pwdHash)
        {
            return queryResult[0][1].as<std::uint64_t>();
        }
        else
        {
            return 0;
        }
    }
    catch (const std::exception& e)
    {
        std::cout << e.what();
        return 0;
    }
}

std::vector<Network::MessageInfo> MessagesRepository::getMessageHistory(const std::uint64_t channelID)
{
    std::vector<Network::MessageInfo> result;

    pTable->changeTable("msgs");
    auto messageHistoryRow =
        pTable->Select()
            ->columns({"msgs.msg_id, msgs.sender_id, msgs.send_time, msgs.msg_type, users.login, COALESCE(text_msgs.msg, "
                       "voice_msgs.voice_name) AS content, COALESCE(NULL, voice_msgs.duration) AS duration"})
            ->join(Utility::SQLJoinType::J_INNER, "channel_msgs", "channel_msgs.msg_id = msgs.msg_id")
            ->join(Utility::SQLJoinType::J_INNER, "users", "users.id = msgs.sender_id")
            ->join(Utility::SQLJoinType::J_LEFT, "text_msgs", "text_msgs.msg_id = msgs.msg_id AND msgs.msg_type = 'T'")
            ->join(Utility::SQLJoinType::J_LEFT, "voice_msgs", "voice_msgs.msg_id = msgs.msg_id AND msgs.msg_type = 'A'")
            ->Where("channel_msgs.channel_id = " + std::to_string(channelID))
            ->execute();

    if (messageHistoryRow.has_value())
    {
        Network::MessageInfo mi = {};
        mi.channelID            = channelID;
        for (auto i = 0; i < messageHistoryRow.value().size(); ++i)
        {
            mi.msgID         = messageHistoryRow.value()[i][0].as<std::uint64_t>();
            mi.senderID      = messageHistoryRow.value()[i][1].as<std::uint64_t>();
            mi.time          = messageHistoryRow.value()[i][2].as<std::string>();
            auto msgInfoType = static_cast<Network::MessageInfoType>(messageHistoryRow.value()[i][3].as<std::string>()[0]);
            mi.userLogin     = messageHistoryRow.value()[i][4].as<std::string>();
            auto contentRef  = messageHistoryRow.value()[i][5];
            auto durationRef = messageHistoryRow.value()[i][6];

            switch (msgInfoType)
            {
                case Network::MessageInfoType::TEXT:
                    assert(durationRef.is_null());
                    mi.setContent(contentRef.as<std::string>());
                    break;
                case Network::MessageInfoType::AUDIO:
                    assert(!durationRef.is_null());
                    mi.setContent(Network::VoiceMessage{contentRef.as<std::string>(), durationRef.as<std::uint16_t>(), {}});
                    break;
                case Network::MessageInfoType::VIDEO:
                    assert(!durationRef.is_null());
                    mi.setContent(Network::VideoMessage{contentRef.as<std::string>(), durationRef.as<std::uint16_t>(), {}});
                    break;
                default:
                    break;
            }
            result.emplace_back(mi);
        }
    }

    return result;
}

Utility::StoringMessageCodes MessagesRepository::storeMessage(const Network::MessageInfo& mi)
{
    const auto firstResult = insertMessageIntoMessagesTable(mi);
    if (!firstResult.has_value())
    {
        std::cerr << "Insert message into 'msgs' table failed" << std::endl;
        return Utility::StoringMessageCodes::FAILED;
    }

    const auto currentMessageID = firstResult.value()[0][0].as<std::uint64_t>();

    const auto secondResult = insertIDsIntoChannelMessagesTable(mi.channelID, currentMessageID);
    if (!secondResult.has_value())
    {
        std::cerr << "Insert message into 'channel_messages' table failed" << std::endl;
        return Utility::StoringMessageCodes::FAILED;
    }

    const auto thirdResult = insertIDIntoMessageReactionsTable(currentMessageID);
    if (!thirdResult.has_value())
    {
        std::cerr << "Insert message into 'msg_reactions' table failed" << std::endl;
        return Utility::StoringMessageCodes::FAILED;
    }

    const auto fourthResult = insertContentIntoConentsTable(mi, currentMessageID);
    if (!fourthResult.has_value())
    {
        std::cerr << "Insert message into 'contents' table failed" << std::endl;
        return Utility::StoringMessageCodes::FAILED;
    }

    return Utility::StoringMessageCodes::SUCCESS;
}

Utility::DeletingMessageCodes MessagesRepository::deleteMessage(const Network::MessageInfo& mi)
{
    using Utility::DeletingMessageCodes;

    pTable->changeTable("msgs");
    pTable->Delete()->Where("msg_id=" + std::to_string(mi.msgID))->execute();

    auto messagesAmountResult = pTable->Select()->columns({"COUNT(*)"})->Where("msg_id=" + std::to_string(mi.msgID))->execute();
    if (messagesAmountResult.value()[0][0].as<std::uint64_t>() == 0)
    {
        return DeletingMessageCodes::SUCCESS;
    }

    return DeletingMessageCodes::FAILED;
}

std::optional<pqxx::result> MessagesRepository::insertMessageIntoMessagesTable(const Network::MessageInfo& mi)
{
    auto       msg_type = static_cast<char>(mi.type);
    std::tuple dataForMsgs{std::pair{"sender_id", mi.senderID}, std::pair{"send_time", mi.time.c_str()}, std::pair{"msg_type", msg_type}};

    pTable->changeTable("msgs");
    return pTable->Insert()->columns(dataForMsgs)->returning({"msg_id"})->execute();
}

std::optional<pqxx::result> MessagesRepository::insertIDsIntoChannelMessagesTable(const std::uint64_t channelID,
                                                                                  const std::uint64_t messageID)
{
    std::tuple dataForChannelMsgs{std::pair{"channel_id", channelID}, std::pair{"msg_id", messageID}};
    pTable->changeTable("channel_msgs");
    return pTable->Insert()->columns(dataForChannelMsgs)->returning({"channel_id"})->execute();
}

std::optional<pqxx::result> MessagesRepository::insertContentIntoConentsTable(const Network::MessageInfo& msi, const std::uint64_t currentMsgID)
{
    switch (msi.type)
    {
        case Network::MessageInfoType::TEXT:
        {
            std::tuple dataForChannelMsgs{std::pair{"msg_id", currentMsgID},
                                          std::pair{"msg", msi.getContent<Network::TextMessage>().text.c_str()}};
            pTable->changeTable("text_msgs");
            return pTable->Insert()->columns(dataForChannelMsgs)->returning({"msg_id"})->execute();
        }
        break;

        case Network::MessageInfoType::AUDIO:
        {
            const auto voiceMessage = msi.getContent<Network::VoiceMessage>();

            std::tuple dataForVoiceMsgs{std::pair{"msg_id", currentMsgID}, std::pair{"voice_name", voiceMessage.fileName.c_str()},
                                        std::pair{"duration", voiceMessage.durationSeconds}};

            pTable->changeTable("voice_msgs");
            return pTable->Insert()->columns(dataForVoiceMsgs)->returning({"msg_id"})->execute();
        }
        break;

        case Network::MessageInfoType::VIDEO:
        {
            const auto videoMessage = msi.getContent<Network::VideoMessage>();

            std::tuple dataForVideoMsgs{std::pair{"msg_id", currentMsgID}, std::pair{"video_name", videoMessage.fileName.c_str()},
                                        std::pair{"duration", videoMessage.durationSeconds}};

            pTable->changeTable("video_msgs");
            return pTable->Insert()->columns(dataForVideoMsgs)->returning({"msg_id"})->execute();
        }
        break;

        default:
        {
        }
    }

    return std::nullopt;
}

std::optional<pqxx::result> MessagesRepository::insertIDIntoMessageReactionsTable(const std::uint64_t messageID)
{
    pTable->changeTable("msg_reactions");
    return pTable->Insert()->columns(std::pair{"msg_id", messageID})->returning({"msg_id"})->execute();
}

Utility::EditingMessageCodes MessagesRepository::editMessage(const Network::MessageInfo& mi)
{
    pTable->changeTable("msgs");

    auto isPresentInTable = pTable->Select()
                                ->columns({"*"})
                                ->Where("msg_id=" + std::to_string(mi.msgID))
                                ->And("msg.sender_id" + std::to_string(mi.senderID))
                                ->execute();
    if (!isPresentInTable.has_value())
    {
        return Utility::EditingMessageCodes::FAILED;
    }

    assert(mi.type == Network::MessageInfoType::TEXT);  // Edit only for text messages
    pTable->Update()
        ->fields(std::pair{"msg", mi.getContent<Network::TextMessage>().text.c_str()})
        ->Where("msg_id=" + std::to_string(mi.msgID))
        ->execute();

    return Utility::EditingMessageCodes::SUCCESS;
}

Utility::RegistrationCodes RegisterRepository::registerUser(const Network::RegistrationInfo& ri)
{
    static UsersAmountFinder finder;
    // Check on existing of login and email in repository.
    if (finder.findUsersAmountWithSameEmail(ri.email) > 0)
    {
        return Utility::RegistrationCodes::EMAIL_ALREADY_EXISTS;
    }
    if (finder.findUsersAmountWithSameLogin(ri.login) > 0)
    {
        return Utility::RegistrationCodes::LOGIN_ALREADY_EXISTS;
    }

    // Data preparation for new user inserting.
    std::tuple userData{std::pair{"email", ri.email}, std::pair{"login", ri.login}, std::pair{"password_hash", ri.passwordHash}};

    // Insert new user.
    pTable->changeTable("users");
    pTable->Insert()->columns(userData)->execute();

    return Utility::RegistrationCodes::SUCCESS;
}

Utility::ReactionMessageCodes MessagesRepository::updateMessageReactions(const Network::MessageInfo& mi)
{
    using Utility::ReactionMessageCodes;

    const std::vector<std::string> reactionNames = {"likes", "dislikes", "fires", "cats", "smiles"};

    auto reactionInfo = std::find_if(mi.reactions.cbegin(), mi.reactions.cend(), [](std::pair<std::uint32_t, std::uint32_t> p) {
        return p.second == std::numeric_limits<std::uint32_t>::max();
    });

    if (reactionInfo == mi.reactions.end())
    {
        std::cerr << "Invalid reaction info supplied" << std::endl;
        return ReactionMessageCodes::FAILED;
    }

    std::uint32_t reactionID   = reactionInfo->first;
    std::string   reactionName = "NULL";

    if (reactionID < reactionNames.size())
    {
        reactionName = reactionNames[reactionID];
    }
    else
    {
        std::cerr << "Reaction with id = \"" << reactionID << "\" is not supported by server";
        return ReactionMessageCodes::FAILED;
    }

    auto adapter = pTable->getAdapter();

    pTable->changeTable("msg_reactions");
    std::optional<pqxx::result> userQueryResult = pTable->Select()
                                                      ->columns({"*"})
                                                      ->Where("msg_id=" + std::to_string(mi.msgID))
                                                      ->And(std::to_string(mi.senderID) + " = ANY(" + reactionName + ");")
                                                      ->execute();

    if (userQueryResult.has_value())
    {
        adapter->query("UPDATE msg_reactions SET " + reactionName + " = array_remove(" + reactionName + ", " + std::to_string(mi.senderID) +
                       ") WHERE msg_id = " + std::to_string(mi.msgID) + ";");
    }
    else
    {
        adapter->query("UPDATE msg_reactions SET " + reactionName + " = array_append(" + reactionName + ", " + std::to_string(mi.senderID) +
                       ") WHERE msg_id = " + std::to_string(mi.msgID) + ";");
    }

    return ReactionMessageCodes::SUCCESS;
}

std::vector<Network::ReplyInfo> RepliesRepository::getReplyHistory(const std::uint64_t channelID)
{
    std::vector<Network::ReplyInfo> result;

    pTable->changeTable("replies");
    auto replyHistoryRow =
        pTable->Select()
            ->columns({"replies.sender_id, replies.msg_id_owner, replies.msg_id_ref, replies.msg, users.login, users.id"})
            ->join(Utility::SQLJoinType::J_INNER, "channel_replies", "channel_replies.msg_id_owner = replies.msg_id_owner")
            ->join(Utility::SQLJoinType::J_INNER, "users", "users.id = replies.sender_id")
            ->Where("channel_replies.channel_id = " + std::to_string(channelID))
            ->execute();
    if (replyHistoryRow.has_value())
    {
        Network::ReplyInfo ri;
        ri.channelID = channelID;
        for (auto i = 0; i < replyHistoryRow.value().size(); ++i)
        {
            ri.senderID   = replyHistoryRow.value()[i][0].as<std::uint64_t>();
            ri.msgIdOwner = replyHistoryRow.value()[i][1].as<std::uint64_t>();
            ri.msgID      = replyHistoryRow.value()[i][2].as<std::uint64_t>();
            ri.message    = replyHistoryRow.value()[i][3].as<std::string>();
            ri.userLogin  = replyHistoryRow.value()[i][4].as<std::string>();
            result.emplace_back(ri);
        }
    }
    return result;
}

Utility::ChannelSubscribingCodes ChannelsRepository::subscribeToChannel(const Network::ChannelSubscriptionInfo& channel)
{
    pTable->changeTable("user_channels");
    auto channel_id              = std::to_string(channel.channelID);
    auto listSubscriptionChannel = pTable->Select()
                                       ->columns({"*"})
                                       ->Where("channel_id = " + channel_id)
                                       ->And("user_id = " + std::to_string(channel.userID))
                                       ->And("(SELECT COUNT(*) from user_channels WHERE channel_id=" + channel_id +
                                             ") < (SELECT user_limit FROM channels WHERE id=" + channel_id + ")")
                                       ->execute();
    if (listSubscriptionChannel.has_value())
    {
        if ((listSubscriptionChannel.value()[0][0].as<std::uint64_t>() == channel.userID) &&
            (listSubscriptionChannel.value()[0][1].as<std::uint64_t>() == channel.channelID))
        {
            return Utility::ChannelSubscribingCodes::CHANNEL_HAS_ALREADY_BEEN_SIGNED;
        }
    }
    // Preparing data for inserting a signed channel.
    std::tuple userData{std::pair{"user_id", channel.userID}, std::pair{"channel_id", channel.channelID}};
    auto       result = pTable->Insert()->columns(userData)->execute();
    if (result.has_value())
    {
        return Utility::ChannelSubscribingCodes::FAILED;
    }
    return Utility::ChannelSubscribingCodes::SUCCESS;
}

std::vector<uint64_t> ChannelsRepository::getChannelSubscriptionList(uint64_t userID)
{
    pTable->changeTable("user_channels");
    /// T\todo add limit check
    auto listSubscriptionChannel = pTable->Select()->columns({"channel_id"})->Where("user_id = " + std::to_string(userID))->execute();
    std::vector<uint64_t> result;
    if (listSubscriptionChannel.has_value())
    {
        for (auto&& value : listSubscriptionChannel.value())
        {
            auto channelID = value[0].as<uint64_t>();
            result.push_back(channelID);
        }
    }
    return result;
}

Utility::StoringReplyCodes RepliesRepository::storeReply(const Network::ReplyInfo& rsi)
{
    const auto firstResult = insertReplyIntoRepliesTable(rsi);
    if (!firstResult.has_value())
    {
        std::cerr << "Insert reply into 'replies' table failed" << std::endl;
        return Utility::StoringReplyCodes::FAILED;
    }

    const auto currentReplyID = firstResult.value()[0][0].as<std::uint64_t>();

    const auto secondResult = insertIDsIntoChannelRepliesTable(rsi.channelID, currentReplyID);
    if (!secondResult.has_value())
    {
        std::cerr << "Insert reply into 'channel_replies' table failed" << std::endl;
        return Utility::StoringReplyCodes::FAILED;
    }

    return Utility::StoringReplyCodes::SUCCESS;
}

std::optional<pqxx::result> RepliesRepository::insertReplyIntoRepliesTable(const Network::ReplyInfo& rsi)
{
    pTable->changeTable("msgs");
    auto lastMsgID = pTable->Select()->columns({"MAX(msg_id)"})->execute();

    std::tuple dataForReplies{std::pair{"sender_id", rsi.senderID}, std::pair{"msg_id_owner", lastMsgID.value()[0][0].as<std::uint64_t>()},
                              std::pair{"msg_id_ref", rsi.msgID}, std::pair{"msg", rsi.message}};

    pTable->changeTable("replies");
    return pTable->Insert()->columns(dataForReplies)->returning({"msg_id_owner"})->execute();
}

std::optional<pqxx::result> RepliesRepository::insertIDsIntoChannelRepliesTable(const std::uint64_t channelID, const std::uint64_t replyID)
{
    std::tuple dataForChannelReplies{std::pair{"channel_id", channelID}, std::pair{"msg_id_owner", replyID}};

    pTable->changeTable("channel_replies");
    return pTable->Insert()->columns(dataForChannelReplies)->returning({"channel_id"})->execute();
}

Utility::DirectMessageStatus DirectMessageRepository::addDirectChat(uint64_t user_id, uint64_t receiverId)
{
    if (user_id == 0) return Utility::DirectMessageStatus::FAILED;
    pTable->changeTable("channels");
    auto adapter   = pTable->getAdapter();
    auto minUserId = std::to_string(std::min(user_id, receiverId));
    auto maxUserId = std::to_string(std::max(user_id, receiverId));
    auto result    = adapter->query(
        "create extension if not exists pgcrypto;\n"
           "INSERT INTO channels VALUES (DEFAULT, encode(digest(concat(" +
        minUserId + ", " + maxUserId + "),'sha384'),'base64')," + minUserId + ",2) ON CONFLICT DO NOTHING;");

    result = adapter->query("SELECT id FROM channels WHERE channel_name=encode(digest(concat(" + minUserId + ", " + maxUserId +
                            "),'sha384'),'base64');");

    if (result.has_value())
    {
        auto cId       = std::any_cast<pqxx::result>(result.value());
        auto channelId = std::to_string(cId[0][0].as<uint64_t>());
        adapter->query("INSERT INTO user_channels VALUES (" + minUserId + "," + channelId + "), (" + maxUserId + "," + channelId +
                       ")ON CONFLICT DO NOTHING;");
        return Utility::DirectMessageStatus::SUCCESS;
    }
    return Utility::DirectMessageStatus::FAILED;
}

}  // namespace DataAccess
