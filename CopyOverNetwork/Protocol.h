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

    void onPingRequest(std::function<awaitable<void>(std::unique_ptr<Message>)> lambda);

    void onSendBytes(std::function<awaitable<void>(std::unique_ptr<Message>)> lambda);

    awaitable<void> sendPingRequest();

    // events
    void onPingRequestEvent(std::function<void()> lambda);
    void onPingResponseEvent(std::function<void()> lambda);

private:
    static std::unique_ptr<Message> prepareMessage(const std::size_t typeOfRequest, const std::size_t sizeOfMessage, void *messageSource);
    static std::unique_ptr<Message> preparePingRequest();

    awaitable<void> handlePing(char *startPosition);
    awaitable<void> handlePingRequest();

private:
    std::function<awaitable<void>(std::unique_ptr<Message>)> pingRequestLambda;
    std::function<awaitable<void>()> pingResponseLambda;

    std::function<awaitable<void>(std::unique_ptr<Message>)> sendBytesLambda;

    // events lambdas
    std::function<void()> pingRequestEvent;
    std::function<void()> pingResponseEvent;
};