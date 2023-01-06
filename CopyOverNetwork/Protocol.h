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

    /// @brief handle package
    /// @param inMessage 
    void onReceivePackage(Message &inMessage);

    void onPingRequest(std::function<void(std::unique_ptr<Message>)> lambda);
    void onPingResponse(std::function<void()> lambda);

private:
    void handlePing(char *startPosition);
    void handlePingRequest(char *startPosition);

private:
    std::function<void(std::unique_ptr<Message>)> pingRequestLambda;
    std::function<void()> pingResponseLambda;
};