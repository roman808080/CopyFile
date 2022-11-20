#include "Server.h"

#include <boost/asio.hpp>
#include <boost/asio/read.hpp>

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

////////////////////////////////////////////////
// Protocol
////////////////////////////////////////////////
// 0 | int |        -> ping (int == 1)
// 1 | str |        -> client name
// 2 | int | str |  -> file name. Int == file_name_size, str == file_name
// 3 | int |        -> permissions
// 4 | int |        -> options. 001 -> allows rewriting for a given client.
// 5 | int | str |  -> file block. First parameter the size of the block, the second char[]
////////////////////////////////////////////////

////////////////////////////////////////////////
// Handler
// private:
// buffer
////////////////////////////////////////////////

namespace
{
    struct Message
    {
        std::array<char, 1024> data{0};
        std::size_t block_size{0};
    };

    class Protocol
    {
    public:
        Protocol()
        {
        }

        void onReceivePackage(const Message &inMessage, Message &outMessage)
        {
        }

    private:
        //
    };

    awaitable<void> handle_client(tcp::socket client)
    {
        Message inMessage{0};
        Protocol protocol;

        while (true)
        {
            co_await boost::asio::async_read(client, buffer(inMessage.data, sizeof(inMessage.block_size)), use_awaitable);
            memcpy(&inMessage.block_size, &inMessage.data, sizeof(inMessage.block_size));

            if (inMessage.block_size > inMessage.data.size())
            {
                throw std::runtime_error("Block size is more then 1024 bytes.");
            }

            co_await boost::asio::async_read(client, buffer(inMessage.data, inMessage.block_size), use_awaitable);

            Message outMessage{0};
            protocol.onReceivePackage(inMessage, outMessage);

            if (outMessage.block_size > 0)
            {
                if (outMessage.block_size > outMessage.data.size())
                {
                    throw std::runtime_error("Block size is more then 1024 bytes.");
                }

                std::array<char, sizeof(std::size_t)> sizeArray{0};
                memcpy(&sizeArray, &outMessage.block_size, sizeof(outMessage.block_size));

                co_await async_write(client, buffer(sizeArray, sizeof(outMessage.block_size)), use_awaitable);
                co_await async_write(client, buffer(outMessage.data, outMessage.block_size), use_awaitable);
            }
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
            "0.0.0.0",
            "5555",
            tcp::resolver::passive);

    tcp::acceptor acceptor(ctx, listen_endpoint);
    co_spawn(ctx, listen(acceptor), detached);

    ctx.run();
}

Server::~Server()
{
}
