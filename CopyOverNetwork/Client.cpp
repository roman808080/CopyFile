#include "Client.h"

#include <iostream>
#include <filesystem>

#include <boost/asio.hpp>
#include <boost/asio/read.hpp>

#include "Protocol.h"

using boost::asio::awaitable;
using boost::asio::buffer;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
using boost::asio::ip::tcp;

namespace fs = std::filesystem;

namespace
{
    class IOClient
    {
    public:
        IOClient(const std::string &host, const std::string &port)
            : host(host),
              port(port),
              connection(ctx)
        {
        }

        void run()
        {
            co_spawn(ctx, start(), detached);
            ctx.run();
        }

        void setClientName(const std::string& clientName)
        {
            this->clientName = clientName;
        }

        void setSource(const std::string& source)
        {
            this->source = source;
        }

        void setDestination(const std::string& destination)
        {
            this->destination = destination;
        }

        awaitable<void> start()
        {
            Protocol protocol;
            auto onPingResponseLambda = []()
            {
                std::cout << "Received ping response." << std::endl;
            };
            protocol.onPingResponseEvent(onPingResponseLambda);

            auto onReceiveBytesLambda = [&](std::size_t bytes) -> awaitable<Message>
            {
                Message message{};
                message.block_size = bytes;
                co_await boost::asio::async_read(connection, buffer(message.data, bytes), use_awaitable);

                co_return std::move(message);
            };

            protocol.onReceiveBytes(onReceiveBytesLambda);
            auto onSendBytesLambda = [&](Message &message) -> awaitable<void>
            {
                co_await async_write(connection, buffer(message.data, message.block_size), use_awaitable);
            };
            protocol.onSendBytes(onSendBytesLambda);

            co_await connect();
            co_await protocol.sendPing();
            co_await protocol.waitForPackage();
            co_await protocol.sendClientName(clientName);

            FileInfo fileInfo{0};

            fs::path pathToSource{source};
            fileInfo.fileSizeInBytes = fs::file_size(pathToSource);

            // TODO: To add implementation for options
            fileInfo.options = 0;

            fs::perms filePermissions(fs::status(pathToSource).permissions());
            fileInfo.permissions = static_cast<unsigned>(filePermissions);

            std::memcpy(fileInfo.pathToFile, destination.data(), destination.size());
            fileInfo.pathToFileSize = destination.size();

            co_await protocol.sendFileInfo(fileInfo);
        }

    private:
        awaitable<void> connect()
        {
            auto target = *tcp::resolver(ctx).resolve(host, port);
            co_await connection.async_connect(target, use_awaitable);
        }

    private:
        const std::string host;
        const std::string port;

        std::string clientName;
        std::string source;
        std::string destination;

        boost::asio::io_context ctx;
        tcp::socket connection;
    };
}

Client::Client(const std::string &host, const std::string &port)
    : host(host),
      port(port)
{
}

void Client::setClientName(const std::string& clientName)
{
    this->clientName = clientName;
}

void Client::setSource(const std::string& source)
{
    this->source = source;
}

void Client::setDestination(const std::string& destination)
{
    this->destination = destination;
}

void Client::run()
{
    IOClient client(host, port);

    client.setClientName(clientName);
    client.setSource(source);
    client.setDestination(destination);

    client.run();
}
