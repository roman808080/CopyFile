#include "Protocol.h"

#include <iostream>
#include <memory.h>

void Protocol::onReceivePackage(Message &inMessage, Message &outMessage)
{
    char *startPosition = inMessage.data.data();

    std::size_t decision{0};
    memcpy(&decision, startPosition, sizeof(decision));
    startPosition += sizeof(decision);

    // TODO: Add enum for decision
    // 1 -> ping
    if (decision == 1)
    {
        std::size_t request_or_response{0};
        memcpy(&request_or_response, startPosition, sizeof(request_or_response));
        startPosition += sizeof(request_or_response);

        // 1 means it is a request
        if (request_or_response == 1)
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
        else if (request_or_response == 2)
        {
            std::cout << "Received ping response." << std::endl;
        }
    }
}