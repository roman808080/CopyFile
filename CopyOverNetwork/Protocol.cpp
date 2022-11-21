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

void Protocol::onReceivePackage(Message &inMessage, Message &outMessage)
{
    char *startPosition = inMessage.data.data();

    std::size_t messageType{0};
    memcpy(&messageType, startPosition, sizeof(messageType));
    startPosition += sizeof(messageType);

    if (static_cast<MessageType>(messageType) == MessageType::Ping)
    {
        std::size_t pingType{0};
        memcpy(&pingType, startPosition, sizeof(pingType));
        startPosition += sizeof(pingType);

        if (static_cast<PingType>(pingType) == PingType::Request)
        {
            std::cout << "Received ping request." << std::endl;

            char *startOutPosition = outMessage.data.data();

            std::size_t typeOfRequest = 1;
            std::size_t response = 2;

            std::size_t totalSize = sizeof(typeOfRequest) + sizeof(response);
            outMessage.block_size = totalSize;

            memcpy(startOutPosition, &typeOfRequest, sizeof(typeOfRequest));
            startOutPosition += sizeof(typeOfRequest);

            memcpy(startOutPosition, &response, sizeof(response));
            startOutPosition += sizeof(response);
        }
        else if (static_cast<PingType>(pingType) == PingType::Response)
        {
            std::cout << "Received ping response." << std::endl;
        }
    }
}