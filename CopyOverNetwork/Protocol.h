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

    awaitable<void> waitForPackage();
    awaitable<void> sendPing();
    awaitable<void> sendClientName(const std::string& clientName);

    void onSendBytes(std::function<awaitable<void>(Message&)> lambda);
    void onReceiveBytes(std::function<awaitable<Message>(std::size_t)> lambda);

    // events
    void onPingRequestEvent(std::function<void()> lambda);
    void onPingResponseEvent(std::function<void()> lambda);
    void onClientNameReceivedEvent(std::function<void(const std::string& clientName)> lambda);

private:
    static Message prepareMessage(const std::size_t typeOfRequest, const std::size_t sizeOfMessage, const void *messageSource);

    awaitable<void> onReceivePackage(Message &inMessage);
    awaitable<void> handlePing(const char *startPosition);
    void handleClientName(const char *startPosition, const std::size_t totalSize);
    awaitable<void> handlePingRequest();

    awaitable<void> sendMessage(Message &message);

private:
    std::function<awaitable<void>(Message&)> sendBytesLambda;
    std::function<awaitable<Message>(std::size_t)> receiveBytesLambda;

    // events lambdas
    std::function<void()> pingRequestEvent;
    std::function<void()> pingResponseEvent;
    std::function<void(const std::string& clientName)> clientNameReceivedEvent;
};