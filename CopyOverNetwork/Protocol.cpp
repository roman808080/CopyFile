#include "Protocol.h"

#include <iostream>
#include <memory.h>

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
: pingRequestLambda([](std::unique_ptr<Message> message){})
{
    //
}

void Protocol::onReceivePackage(Message &inMessage, Message &outMessage)
{
    char *startPosition = inMessage.data.data();

    std::size_t messageType{0};
    memcpy(&messageType, startPosition, sizeof(messageType));
    startPosition += sizeof(messageType);

    switch (static_cast<MessageType>(messageType))
    {
    case MessageType::Ping:
        handlePing(startPosition, outMessage);
        break;

    default:
        std::runtime_error("Unsupported Message Type");
    }
}

void Protocol::onReceivePackage(std::function<void(std::unique_ptr<Message>)> lambda)
{
    pingRequestLambda = lambda;
}

void Protocol::handlePing(char *startPosition, Message &outMessage)
{
    std::size_t pingType{0};
    memcpy(&pingType, startPosition, sizeof(pingType));
    startPosition += sizeof(pingType);

    switch (static_cast<PingType>(pingType))
    {
    case PingType::Request:
        handlePingRequest(startPosition, outMessage);
        break;
    case PingType::Response:
        std::cout << "Received ping response." << std::endl;
        break;

    default:
        std::runtime_error("Unsupported Ping Type");
    }
}

void Protocol::handlePingRequest(char *startPosition, Message &outMessage)
{
    std::cout << "Received ping request." << std::endl;

    char *startOutPosition = outMessage.data.data();

    std::size_t typeOfRequest = static_cast<std::size_t>(MessageType::Ping);
    std::size_t response = static_cast<std::size_t>(PingType::Response);

    std::size_t totalSize = sizeof(typeOfRequest) + sizeof(response);
    outMessage.block_size = totalSize;

    memcpy(startOutPosition, &typeOfRequest, sizeof(typeOfRequest));
    startOutPosition += sizeof(typeOfRequest);

    memcpy(startOutPosition, &response, sizeof(response));
    startOutPosition += sizeof(response);
}