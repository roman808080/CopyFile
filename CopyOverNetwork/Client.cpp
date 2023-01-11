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
            auto onPingResponseLambda = []() -> awaitable<void>
            {
                // The message for debugging purposes. TODO: to remove the next line
                std::cout << "Received ping response." << std::endl;
                co_return;
            };
            protocol.onPingResponse(onPingResponseLambda);

            co_await connect();
            co_await send_ping();

            auto inMessage = co_await get_next_message();
            co_await protocol.onReceivePackage(inMessage);
        }

    private:
        awaitable<void> connect()
        {
            auto target = *tcp::resolver(ctx).resolve(host, port);
            co_await connection.async_connect(target, use_awaitable);
        }

        awaitable<Message> get_next_message()
        {
            Message inMessage{0};

            co_await boost::asio::async_read(connection, buffer(inMessage.data, sizeof(inMessage.block_size)), use_awaitable);
            std::memcpy(&inMessage.block_size, &inMessage.data, sizeof(inMessage.block_size));

            if (inMessage.block_size > inMessage.data.size())
            {
                throw std::runtime_error("Block size is more then 1024 bytes.");
            }

            co_await boost::asio::async_read(connection, buffer(inMessage.data, inMessage.block_size), use_awaitable);
            co_return inMessage;
        }

        awaitable<void> send_ping()
        {
            auto message(Protocol::preparePingRequest());

            std::array<char, sizeof(std::size_t)> sizeArray{0};
            std::memcpy(&sizeArray, &message->block_size, sizeof(message->block_size));

            co_await async_write(connection, buffer(sizeArray, sizeof(message->block_size)), use_awaitable);
            co_await async_write(connection, buffer(message->data, message->block_size), use_awaitable);
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
