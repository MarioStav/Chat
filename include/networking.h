/*
* Name: Mario Stavarache
* Class: 5BHIF
* ProjectNr.: 15
* MatNr.: i14095
* File: networking.h
*/

#pragma once

#include <google/protobuf/message.h>
#include <asio.hpp>
#include <unordered_map>
#include <typeindex>

#include "Chat.pb.h"

using namespace asio::ip;
using asio::buffer;
using asio::streambuf;
using asio::write;
using asio::ip::tcp;

using std::istream;
using std::ostream;

// Extracts integral value of enum class variants
template <typename E>
constexpr auto toUnderlying(E e) noexcept
{
    return static_cast<u_int8_t>(static_cast<std::underlying_type_t<E>>(e));
}

namespace networking
{
const int SEND_OK{0};
const int SEND_ERR{1};

enum class MessageType
{
    ChatMessage = 1,
    SignIn = 2,
    SignOut = 3,
    Unauthorized = 4,
    joinChannel = 5,
    leaveChannel = 6
};

const std::unordered_map<std::type_index, MessageType> typeMapping{
    {typeid(Chat::ChatMessage), MessageType::ChatMessage},
    {typeid(Chat::SignIn), MessageType::SignIn},
    {typeid(Chat::SignOut), MessageType::SignOut},
    {typeid(Chat::Unauthorized), MessageType::Unauthorized},
    {typeid(Chat::joinChannel), MessageType::joinChannel},
    {typeid(Chat::leaveChannel), MessageType::leaveChannel}};

inline int sendProto(tcp::socket &socket, google::protobuf::Message &message)
{
    u_int8_t messageType{toUnderlying(typeMapping.at(typeid(message)))};
    u_int64_t messageSize{message.ByteSizeLong()};

    asio::write(socket, buffer(&messageType, sizeof(messageType)));
    asio::write(socket, buffer(&messageSize, sizeof(messageSize)));

    asio::streambuf streamBuffer;
    ostream outputStream(&streamBuffer);
    message.SerializeToOstream(&outputStream);
    asio::write(socket, streamBuffer);
    return SEND_OK;
}

inline int receiveProtoMessageType(tcp::socket &socket, MessageType &messageType)
{
    u_int8_t messageTypeRaw;

    socket.receive(buffer(&messageTypeRaw, sizeof(messageTypeRaw)), 0);
    messageType = static_cast<MessageType>(messageTypeRaw);

    return SEND_OK;
}

inline int receiveProtoMessage(tcp::socket &socket, google::protobuf::Message &message)
{
    u_int64_t messageSize;
    socket.receive(buffer(&messageSize, sizeof(messageSize)), 0);

    asio::streambuf streamBuffer;
    asio::streambuf::mutable_buffers_type mutableBuffer{streamBuffer.prepare(messageSize)};

    streamBuffer.commit(read(socket, mutableBuffer));

    istream inputStream{&streamBuffer};
    message.ParseFromIstream(&inputStream);

    return SEND_OK;
}
} // namespace networking
