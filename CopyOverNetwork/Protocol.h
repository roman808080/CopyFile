#pragma once
#include <array>

struct Message
{
    std::array<char, 1024> data{0};
    std::size_t block_size{0};
};

class Protocol
{
public:
    Protocol()
    {
    }

    void onReceivePackage(Message &inMessage, Message &outMessage);

private:
    void handlePing(char *startPosition, Message &outMessage);

private:
    //
};