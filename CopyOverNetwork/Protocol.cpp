#include "Protocol.h"

#include <cstring>
#include <stdexcept>

////////////////////////////////////////////////
// Protocol
////////////////////////////////////////////////
// 1 | int |        -> ping (int == 1)
// 2 | str |        -> client name
// 3 | int | str | int | -> file name. file_name_size -> file_name -> permissions -> size -> options.
//                              * permissions -- e.g. 777 or 600
//                              * options -- e.g. 001 -> allows rewriting for a given client.
// 4 | int | str | -> file block. First parameter the size of the block, the second char[]
// 5 | int | str | -> exception. Size of the error message and the message.
////////////////////////////////////////////////

namespace
{
    enum class MessageType : std::size_t
    {
        Ping = 1,
        ClientName = 2,
        FileInfo = 3,
        FileBlock = 4,
    };

    enum class PingType : std::size_t
    {
        Request = 1,
        Response = 2,
    };
}

Protocol::Protocol()
    : sendBytesLambda([](Message &) -> awaitable<void>
                      { co_return; }),
      receiveBytesLambda([](std::size_t) -> awaitable<Message>
                         { co_return Message{}; }),
      pingRequestEvent([]() {}),
      pingResponseEvent([]() {}),
      clientNameReceivedEvent([](const std::string& clientName) {}),
      fileInfoEvent([](const FileInfo& fileInfo) {})
{
}

awaitable<void> Protocol::waitForPackage()
{
    std::size_t nextMessageSize{0};
    auto messageSizeOfNextMessage(co_await receiveBytesLambda(sizeof(nextMessageSize)));
    std::memcpy(&nextMessageSize, &messageSizeOfNextMessage.data, sizeof(nextMessageSize));

    auto message(co_await receiveBytesLambda(nextMessageSize));
    co_await onReceivePackage(message);
}

awaitable<void> Protocol::sendClientName(const std::string &clientName)
{
    std::size_t typeOfRequest = static_cast<std::size_t>(MessageType::ClientName);
    auto message(Protocol::prepareMessage(typeOfRequest, clientName.size(), clientName.data()));
    co_await sendMessage(message);
}

awaitable<void> Protocol::sendFileInfo(const FileInfo& fileInfo)
{
    std::size_t typeOfRequest = static_cast<std::size_t>(MessageType::FileInfo);
    auto message(Protocol::prepareMessage(typeOfRequest, sizeof(fileInfo), &fileInfo));
    co_await sendMessage(message);
}

awaitable<void> Protocol::onReceivePackage(Message &inMessage)
{
    char *startPosition = inMessage.data.data();

    std::size_t messageType{0};
    std::memcpy(&messageType, startPosition, sizeof(messageType));

    // Updating the start position and the total size after we have read the first block
    startPosition += sizeof(messageType);
    std::size_t messageSize = inMessage.block_size - sizeof(messageType);

    switch (static_cast<MessageType>(messageType))
    {
    case MessageType::Ping:
        co_await handlePing(startPosition);
        break;

    case MessageType::ClientName:
        handleClientName(startPosition, messageSize);
        break;

    case MessageType::FileInfo:
        handleFileInfo(startPosition, messageSize);
        break;

    default:
        std::runtime_error("Unsupported Message Type");
    }

    co_return;
}

void Protocol::onSendBytes(const std::function<awaitable<void>(Message &)>& lambda)
{
    sendBytesLambda = lambda;
}

void Protocol::onReceiveBytes(const std::function<awaitable<Message>(std::size_t)>& lambda)
{
    receiveBytesLambda = lambda;
}

awaitable<void> Protocol::handlePing(const char *startPosition)
{
    std::size_t pingType{0};
    std::memcpy(&pingType, startPosition, sizeof(pingType));

    switch (static_cast<PingType>(pingType))
    {
    case PingType::Request:
        pingRequestEvent();
        co_await handlePingRequest();
        co_return;

    case PingType::Response:
        pingResponseEvent();
        co_return;

    default:
        std::runtime_error("Unsupported Ping Type");
    }
}

void Protocol::handleClientName(const char *startPosition, const std::size_t totalSize)
{
    std::string clientName(startPosition, totalSize);
    clientNameReceivedEvent(clientName);
}

void Protocol::handleFileInfo(const char *startPosition, const std::size_t totalSize)
{
    FileInfo fileInfo{0};
    std::memcpy(&fileInfo, startPosition, totalSize);
    fileInfoEvent(fileInfo);
}

awaitable<void> Protocol::sendPing()
{
    std::size_t typeOfRequest = static_cast<std::size_t>(MessageType::Ping);
    std::size_t request = static_cast<std::size_t>(PingType::Request);

    auto message(Protocol::prepareMessage(typeOfRequest, sizeof(request), &request));
    co_await sendMessage(message);
}

void Protocol::onPingRequestEvent(const std::function<void()>& lambda)
{
    pingRequestEvent = lambda;
}

void Protocol::onPingResponseEvent(const std::function<void()>& lambda)
{
    pingResponseEvent = lambda;
}

void Protocol::onClientNameReceivedEvent(const std::function<void(const std::string &clientName)>& lambda)
{
    clientNameReceivedEvent = lambda;
}

void Protocol::onFileInfoEvent(const std::function<void(const FileInfo& fileInfo)>& lambda)
{
    fileInfoEvent = lambda;
}

Message Protocol::prepareMessage(const std::size_t typeOfRequest, const std::size_t sizeOfMessage, const void *messageSource)
{
    Message message{};

    // Getting the start position to which we will copy our data
    char *startOutPosition = message.data.data();

    // Calculating the total size of the message
    std::size_t totalSize = sizeof(typeOfRequest) + sizeOfMessage;
    message.block_size = totalSize;

    // Copying the type of request
    std::memcpy(startOutPosition, &typeOfRequest, sizeof(typeOfRequest));

    // Step over because this position was already used
    startOutPosition += sizeof(typeOfRequest);

    // Copying the message
    std::memcpy(startOutPosition, messageSource, sizeOfMessage);

    return std::move(message);
}

awaitable<void> Protocol::handlePingRequest()
{
    std::size_t typeOfRequest = static_cast<std::size_t>(MessageType::Ping);
    std::size_t response = static_cast<std::size_t>(PingType::Response);

    auto message(prepareMessage(typeOfRequest, sizeof(response), &response));
    co_await sendMessage(message);
}

awaitable<void> Protocol::sendMessage(Message &message)
{
    Message nextMessageSize{};

    nextMessageSize.block_size = sizeof(message.block_size);
    std::memcpy(&nextMessageSize.data, &message.block_size, nextMessageSize.block_size);

    co_await sendBytesLambda(nextMessageSize);
    co_await sendBytesLambda(message);
}