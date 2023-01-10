#pragma once
#include <array>
#include <functional>
#include <memory>
#include <boost/asio.hpp>

using boost::asio::awaitable;

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
    awaitable<void> onReceivePackage(Message &inMessage);

    void onPingRequest(std::function<void(std::unique_ptr<Message>)> lambda);
    void onPingResponse(std::function<void()> lambda);

private:
    awaitable<void> handlePing(char *startPosition);
    awaitable<void> handlePingRequest(char *startPosition);

private:
    std::function<void(std::unique_ptr<Message>)> pingRequestLambda;
    std::function<void()> pingResponseLambda;
};