#include "Server.h"

#include <boost/asio.hpp>
#include <boost/asio/read.hpp>
#include <iostream>

#include "Protocol.h"

using boost::asio::awaitable;
using boost::asio::buffer;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
using boost::asio::ip::tcp;

////////////////////////////////////////////////
// On TCP Level
////////////////////////////////////////////////
// | size | char[] | -> the Client class has only shallow understanding about our protocol
////////////////////////////////////////////////

namespace
{
    awaitable<void> handle_client(tcp::socket client)
    {
        Message inMessage{0};
        Protocol protocol;

        auto onReceiveBytesLambda = [&](std::size_t bytes) -> awaitable<Message>
        {
            Message message{};
            message.block_size = bytes;
            co_await boost::asio::async_read(client, buffer(message.data, bytes), use_awaitable);

            co_return std::move(message);
        };
        protocol.onReceiveBytes(onReceiveBytesLambda);

        auto onSendBytesLambda = [&](Message &message) -> awaitable<void>
        {
            co_await async_write(client, buffer(message.data, message.block_size), use_awaitable);
        };
        protocol.onSendBytes(onSendBytesLambda);

        auto onPingRequestEvent = []()
        {
            std::cout << "Received ping request." << std::endl;
        };
        protocol.onPingRequestEvent(onPingRequestEvent);

        auto onPingResponseLambda = []()
        {
            std::cout << "Received ping response." << std::endl;
        };
        protocol.onPingResponseEvent(onPingResponseLambda);

        auto onReceiveClientNames = [](const std::string& clientName)
        {
            std::cout << "Received the client name: " << clientName << std::endl;
        };
        protocol.onClientNameReceivedEvent(onReceiveClientNames);

        auto onReceiveFileInfo = [](const FileInfo& fileInfo)
        {
            std::string pathToFile(fileInfo.pathToFile, fileInfo.pathToFileSize);

            std::cout << "Received the file info: " << std::endl;
            std::cout << "Path to the file: " << pathToFile << std::endl;
            std::cout << "The file size in bytes: " << fileInfo.fileSizeInBytes << std::endl;
            std::cout << "Options: " << fileInfo.options << std::endl;
            std::cout << "Permissions: " << fileInfo.permissions << std::endl;
        };
        protocol.onFileInfoEvent(onReceiveFileInfo);

        while (true)
        {
            co_await protocol.waitForPackage();
        }
    }

    awaitable<void> listen(tcp::acceptor &acceptor)
    {
        for (;;)
        {
            auto client = co_await acceptor.async_accept(use_awaitable);

            auto ex = client.get_executor();
            co_spawn(ex, handle_client(std::move(client)), detached);
        }
    }
}

Server::Server(const std::string &host, const std::string &port)
    : host(host),
      port(port)
{
}

void Server::run()
{
    boost::asio::io_context ctx;

    auto listen_endpoint =
        *tcp::resolver(ctx).resolve(
            host, port,
            tcp::resolver::passive);

    tcp::acceptor acceptor(ctx, listen_endpoint);
    co_spawn(ctx, listen(acceptor), detached);

    ctx.run();
}
