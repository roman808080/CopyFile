#include "Client.h"

#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/read.hpp>

using boost::asio::awaitable;
using boost::asio::buffer;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
using boost::asio::ip::tcp;

namespace
{
    struct Message
    {
        std::array<char, 1024> data{0};
        std::size_t block_size{0};
    };

    awaitable<void> connect_to_server(boost::asio::io_context &ctx, const std::string &host, const std::string &port)
    {
        tcp::socket server(ctx);
        auto target = *tcp::resolver(ctx).resolve(host, port);
        co_await server.async_connect(target, use_awaitable);

        Message outMessage{0};
        auto startOutPosition = &outMessage.data;

        std::size_t typeOfRequest = 1;
        std::size_t request = 1;
        std::size_t totalSize = sizeof(typeOfRequest) + sizeof(request);

        memcpy(startOutPosition, &typeOfRequest, sizeof(typeOfRequest));
        startOutPosition += sizeof(typeOfRequest);

        memcpy(startOutPosition, &request, sizeof(request));
        startOutPosition += sizeof(request);

        std::array<char, sizeof(std::size_t)> sizeArray{0};
        outMessage.block_size = totalSize;
        memcpy(&sizeArray, &outMessage.block_size, sizeof(outMessage.block_size));

        co_await async_write(server, buffer(sizeArray, sizeof(outMessage.block_size)), use_awaitable);
        co_await async_write(server, buffer(outMessage.data, outMessage.block_size), use_awaitable);
    }
}

Client::Client(const std::string &host, const std::string &port)
    : host(host),
      port(port)
{
}

void Client::run()
{
    boost::asio::io_context ctx;
    co_spawn(ctx, connect_to_server(ctx, host, port), detached);

    ctx.run();
}

Client::~Client()
{
}