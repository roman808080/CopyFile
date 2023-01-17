#pragma once
#include <array>
#include <functional>
#include <memory>
#include <boost/asio.hpp>

using boost::asio::awaitable;

const std::size_t kMaxMessageSize = 1024;

using MessageBytes = std::array<char, kMaxMessageSize>;
using Permissions = unsigned short int;

struct Message
{
    MessageBytes data{0};
    std::size_t block_size{0};
};

struct FileInfo
{
    char pathToFile[kMaxMessageSize]{0};
    std::size_t fileSizeInBytes{0};
    std::size_t options{0};
    Permissions permissions{0};
};

class Protocol
{
public:
    Protocol();

    awaitable<void> waitForPackage();
    awaitable<void> sendPing();
    awaitable<void> sendClientName(const std::string &clientName);
    awaitable<void> sendFileInfo(const FileInfo& fileInfo);

    void onSendBytes(const std::function<awaitable<void>(Message &)>& lambda);
    void onReceiveBytes(const std::function<awaitable<Message>(std::size_t)>& lambda);

    // events
    void onPingRequestEvent(const std::function<void()>& lambda);
    void onPingResponseEvent(const std::function<void()>& lambda);
    void onClientNameReceivedEvent(const std::function<void(const std::string &clientName)>& lambda);
    void onFileInfoEvent(const std::function<void(const FileInfo& fileInfo)>& lambda);

private:
    static Message prepareMessage(const std::size_t typeOfRequest, const std::size_t sizeOfMessage, const void *messageSource);

    awaitable<void> onReceivePackage(Message &inMessage);
    awaitable<void> handlePing(const char *startPosition);
    void handleClientName(const char *startPosition, const std::size_t totalSize);
    void handleFileInfo(const char *startPosition, const std::size_t totalSize);
    awaitable<void> handlePingRequest();

    awaitable<void> sendMessage(Message &message);

private:
    std::function<awaitable<void>(Message &)> sendBytesLambda;
    std::function<awaitable<Message>(std::size_t)> receiveBytesLambda;

    // events lambdas
    std::function<void()> pingRequestEvent;
    std::function<void()> pingResponseEvent;
    std::function<void(const std::string &clientName)> clientNameReceivedEvent;
    std::function<void(const FileInfo& fileInfo)> fileInfoEvent;
};