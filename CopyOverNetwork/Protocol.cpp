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
        FileName = 3,
        FileBlock = 4,
    };

    enum class PingType : std::size_t
    {
        Request = 1,
        Response = 2,
    };
}

Protocol::Protocol()
    : sendBytesLambda([](std::unique_ptr<Message>) -> awaitable<void>
                      { co_return; }),
      pingRequestEvent([]() {}),
      pingResponseEvent([]() {})
{
}

awaitable<void> Protocol::onReceivePackage(Message &inMessage)
{
    char *startPosition = inMessage.data.data();

    std::size_t messageType{0};
    std::memcpy(&messageType, startPosition, sizeof(messageType));
    startPosition += sizeof(messageType);

    switch (static_cast<MessageType>(messageType))
    {
    case MessageType::Ping:
        co_await handlePing(startPosition);
        break;

    default:
        std::runtime_error("Unsupported Message Type");
    }

    co_return;
}

void Protocol::onSendBytes(std::function<awaitable<void>(std::unique_ptr<Message>)> lambda)
{
    sendBytesLambda = lambda;
}

awaitable<void> Protocol::handlePing(char *startPosition)
{
    std::size_t pingType{0};
    std::memcpy(&pingType, startPosition, sizeof(pingType));
    startPosition += sizeof(pingType);

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

awaitable<void> Protocol::sendPingRequest()
{
    auto message(Protocol::preparePingRequest());
    auto nextMessageSize(std::make_unique<Message>());

    nextMessageSize->block_size = sizeof(message->block_size);
    std::memcpy(&nextMessageSize->data, &message->block_size, nextMessageSize->block_size);

    co_await sendBytesLambda(std::move(nextMessageSize));
    co_await sendBytesLambda(std::move(message));
}

void Protocol::onPingRequestEvent(std::function<void()> lambda)
{
    pingRequestEvent = lambda;
}

void Protocol::onPingResponseEvent(std::function<void()> lambda)
{
    pingResponseEvent = lambda;
}

std::unique_ptr<Message> Protocol::prepareMessage(const std::size_t typeOfRequest, const std::size_t sizeOfMessage, void *messageSource)
{
    auto message = std::make_unique<Message>();

    // Getting the start position to which we will copy our data
    char *startOutPosition = message->data.data();

    // Calculating the total size of the message
    std::size_t totalSize = sizeof(typeOfRequest) + sizeOfMessage;
    message->block_size = totalSize;

    // Copying the type of request
    std::memcpy(startOutPosition, &typeOfRequest, sizeof(typeOfRequest));

    // Step over because this position was already used
    startOutPosition += sizeof(typeOfRequest);

    // Copying the message
    std::memcpy(startOutPosition, messageSource, sizeOfMessage);

    return std::move(message);
}

std::unique_ptr<Message> Protocol::preparePingRequest()
{
    std::size_t typeOfRequest = static_cast<std::size_t>(MessageType::Ping);
    std::size_t request = static_cast<std::size_t>(PingType::Request);

    std::size_t totalSize = sizeof(typeOfRequest) + sizeof(request);

    auto message(Protocol::prepareMessage(typeOfRequest, sizeof(request), &request));

    return std::move(message);
}

awaitable<void> Protocol::handlePingRequest()
{
    std::size_t typeOfRequest = static_cast<std::size_t>(MessageType::Ping);
    std::size_t response = static_cast<std::size_t>(PingType::Response);

    auto message(prepareMessage(typeOfRequest, sizeof(response), &response));
    auto nextMessageSize(std::make_unique<Message>());

    nextMessageSize->block_size = sizeof(message->block_size);
    std::memcpy(&nextMessageSize->data, &message->block_size, nextMessageSize->block_size);

    co_await sendBytesLambda(std::move(nextMessageSize));
    co_await sendBytesLambda(std::move(message));
}