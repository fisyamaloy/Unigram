#pragma once

#include "Utility/WarningSuppression.hpp"
suppressWarning(4996, "-Wdeprecated-declarations") suppressWarning(4458, "-Wshadow")
#include <yas/binary_iarchive.hpp>
#include <yas/binary_oarchive.hpp>
#include <yas/buffers.hpp>
#include <yas/detail/type_traits/type_traits.hpp>
#include <yas/mem_streams.hpp>
#include <yas/object.hpp>
#include <yas/serialize.hpp>
#include <yas/std_types.hpp>
#include <yas/types/std/string.hpp>
#include <yas/types/std/vector.hpp>
    restoreWarning restoreWarning

    constexpr static std::size_t flags = yas::mem | yas::binary | yas::no_header;

#include "Network/Primitives.hpp"

namespace Network
{
template <typename Archive>
void serialize(Archive& ar, Network::MessageInfo& o)
{
    ar & o.channelID & o.senderID & o.msgID & o.reactions & o.time & o.userLogin & o.type;
    switch (o.type)
    {
        case Network::MessageInfoType::TEXT:
        {
            Network::TextMessage textMsg;
            if constexpr (!std::is_same_v<typename Archive::stream_type, yas::mem_istream>)
            {
                textMsg = std::get<Network::TextMessage>(o.content);
            }
            ar & textMsg;
            if constexpr (std::is_same_v<typename Archive::stream_type, yas::mem_istream>)
            {
                o.content = std::move(textMsg);
            }
        }
        break;

        case Network::MessageInfoType::AUDIO:
        {
            Network::VoiceMessage voiceMsg;
            if constexpr (!std::is_same_v<typename Archive::stream_type, yas::mem_istream>)
            {
                voiceMsg = std::get<Network::VoiceMessage>(o.content);
            }
            ar & voiceMsg;
            if constexpr (std::is_same_v<typename Archive::stream_type, yas::mem_istream>)
            {
                o.content = std::move(voiceMsg);
            }
        }
        break;

        case Network::MessageInfoType::VIDEO:
        {
            Network::VideoMessage videoMsg;
            if constexpr (!std::is_same_v<typename Archive::stream_type, yas::mem_istream>)
            {
                videoMsg = std::get<Network::VideoMessage>(o.content);
            }
            ar & videoMsg;
            if constexpr (std::is_same_v<typename Archive::stream_type, yas::mem_istream>)
            {
                o.content = std::move(videoMsg);
            }
        }
        break;

        default:
            throw std::runtime_error("Unknown message type during serialization");
    }
}

/** @enum SerializedState
 *  @brief Successful or not result of serialization/deserialization
 */
enum class SerializedState
{
    SUCCESS,  /// successful serialization/deserialization
    FAILURE   /// unsuccessful serialization/deserialization
};

/** @class YasSerializer
 *  @brief binary serialization class using YAS library.
 */
class YasSerializer
{
    YasSerializer()                                = delete;
    YasSerializer(const YasSerializer&)            = delete;
    YasSerializer(YasSerializer&&)                 = delete;
    YasSerializer& operator=(const YasSerializer&) = delete;
    YasSerializer& operator=(YasSerializer&&)      = delete;

private:
    // constexpr static std::size_t flags = yas::mem | yas::binary | yas::no_header;

public:
    /**
     * @brief Method for binary serialization of messages.
     * @param msg - buffer that will contain serialized message data.
     * @param data - variable that contains data that should be serialized.
     */
    template <typename T>
    static SerializedState serialize(yas::shared_buffer& msg, const T& data)
    {
        try
        {
            suppressWarning(4127, "-Wtype-limits") msg = yas::save<flags>(data);
            restoreWarning
        }
        catch (const std::exception& e)
        {
            std::cout << "Serialization error\n";
            std::cout << e.what() << '\n';

            return SerializedState::FAILURE;
        }

        return SerializedState::SUCCESS;
    }

    /**
     * @brief Method for binary deserialization of messages.
     * @param source - variable that contains data that should be deserialized.
     * @param data - variable that will contain deserialized message data.
     */
    template <typename T>
    static SerializedState deserialize(const yas::shared_buffer source, T& data)
    {
        data = T();
        try
        {
            suppressWarning(4127, "-Wtype-limits") yas::load<flags>(source, data);
            restoreWarning
        }
        catch (const std::exception& e)
        {
            std::cout << "Deserialization error\n";
            std::cout << e.what() << '\n';

            return SerializedState::FAILURE;
        }

        return SerializedState::SUCCESS;
    }
};
}  // namespace Network
