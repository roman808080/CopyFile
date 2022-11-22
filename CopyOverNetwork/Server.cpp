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
    awaitable<void> sendMessage(std::unique_ptr<Message> message, tcp::socket& client)
    {
        if (message->block_size > message->data.size())
        {
            throw std::runtime_error("Block size is more then 1024 bytes.");
        }

        std::array<char, sizeof(std::size_t)> sizeArray{0};
        std::memcpy(&sizeArray, &message->block_size, sizeof(message->block_size));

        co_await async_write(client, buffer(sizeArray, sizeof(message->block_size)), use_awaitable);
        co_await async_write(client, buffer(message->data, message->block_size), use_awaitable);
    }

    awaitable<void> handle_client(tcp::socket client)
    {
        Message inMessage{0};
        Protocol protocol;

        auto onPingRequestLambda = [&client](std::unique_ptr<Message> message)
        {
            // The message for debugging purposes. TODO: to remove the next line
            std::cout << "Received ping request." << std::endl;

            auto ex = client.get_executor();
            co_spawn(ex, sendMessage(std::move(message), client), detached);
        };
        protocol.onPingRequest(onPingRequestLambda);

        auto onPingResponseLambda = []()
        {
            // The message for debugging purposes. TODO: to remove the next line
            std::cout << "Received ping response." << std::endl;
        };
        protocol.onPingResponse(onPingResponseLambda);

        while (true)
        {
            co_await boost::asio::async_read(client, buffer(inMessage.data, sizeof(inMessage.block_size)), use_awaitable);
            std::memcpy(&inMessage.block_size, &inMessage.data, sizeof(inMessage.block_size));

            if (inMessage.block_size > inMessage.data.size())
            {
                throw std::runtime_error("Block size is more then 1024 bytes.");
            }

            co_await boost::asio::async_read(client, buffer(inMessage.data, inMessage.block_size), use_awaitable);
            protocol.onReceivePackage(inMessage);
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

Server::~Server()
{
}
