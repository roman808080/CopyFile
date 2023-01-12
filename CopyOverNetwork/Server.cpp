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

        auto onSendBytesLambda = [&](Message& message) -> awaitable<void>
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

        while (true)
        {
            co_await boost::asio::async_read(client, buffer(inMessage.data, sizeof(inMessage.block_size)), use_awaitable);
            std::memcpy(&inMessage.block_size, &inMessage.data, sizeof(inMessage.block_size));

            if (inMessage.block_size > inMessage.data.size())
            {
                throw std::runtime_error("Block size is more then 1024 bytes.");
            }

            co_await boost::asio::async_read(client, buffer(inMessage.data, inMessage.block_size), use_awaitable);
            co_await protocol.onReceivePackage(inMessage);
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
