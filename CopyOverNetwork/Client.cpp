#include "Client.h"

#include <iostream>

#include <boost/asio.hpp>
#include <boost/asio/read.hpp>

#include "Protocol.h"

using boost::asio::awaitable;
using boost::asio::buffer;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
using boost::asio::ip::tcp;

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

        boost::asio::io_context ctx;
        tcp::socket connection;
    };
}

Client::Client(const std::string &host, const std::string &port)
    : host(host),
      port(port)
{
}

void Client::run()
{
    IOClient client(host, port);
    client.run();
}
