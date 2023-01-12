#pragma once
#include <array>
#include <functional>
#include <memory>
#include <boost/asio.hpp>

using boost::asio::awaitable;

using MessageBytes = std::array<char, 1024>;

struct Message
{
    MessageBytes data{0};
    std::size_t block_size{0};
};

class Protocol
{
public:
    Protocol();

    /// @brief handle package
    /// @param inMessage 
    awaitable<void> onReceivePackage(Message &inMessage);

    void onSendBytes(std::function<awaitable<void>(Message&)> lambda);

    awaitable<void> sendPing();

    // events
    void onPingRequestEvent(std::function<void()> lambda);
    void onPingResponseEvent(std::function<void()> lambda);

private:
    static Message prepareMessage(const std::size_t typeOfRequest, const std::size_t sizeOfMessage, void *messageSource);
    static Message preparePingRequest();

    awaitable<void> handlePing(char *startPosition);
    awaitable<void> handlePingRequest();

    awaitable<void> sendMessage(Message &message);

private:
    std::function<awaitable<void>(Message&)> sendBytesLambda;

    // events lambdas
    std::function<void()> pingRequestEvent;
    std::function<void()> pingResponseEvent;
};