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

    static std::unique_ptr<Message> prepareMessage(const std::size_t typeOfRequest, const std::size_t sizeOfMessage, void *messageSource);

    void onPingRequest(std::function<awaitable<void>(std::unique_ptr<Message>)> lambda);
    void onPingResponse(std::function<awaitable<void>()> lambda);

private:
    awaitable<void> handlePing(char *startPosition);
    awaitable<void> handlePingRequest();

private:
    std::function<awaitable<void>(std::unique_ptr<Message>)> pingRequestLambda;
    std::function<awaitable<void>()> pingResponseLambda;
};