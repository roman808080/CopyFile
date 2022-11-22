#pragma once
#include <array>
#include <functional>
#include <memory>

struct Message
{
    std::array<char, 1024> data{0};
    std::size_t block_size{0};
};

class Protocol
{
public:
    Protocol();

    void onReceivePackage(Message &inMessage, Message &outMessage);
    void onReceivePackage(std::function<void(std::unique_ptr<Message>)> lambda);

private:
    void handlePing(char *startPosition, Message &outMessage);
    void handlePingRequest(char *startPosition, Message &outMessage);

private:
    std::function<void(std::unique_ptr<Message>)> pingRequestLambda;
};