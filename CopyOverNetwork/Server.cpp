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
    class Protocol
    {
    public:
        Protocol()
        {
        }

        void onReceivePackage(std::size_t packageSize, const std::array<char, 1024> &package)
        {
        }

    private:
        //
    };

    awaitable<void> handle_client(tcp::socket client)
    {
        std::array<char, 1024> data{0};
        Protocol protocol;

        while (true)
        {
            std::size_t next_block_size = 0;

            co_await boost::asio::async_read(client, buffer(data, sizeof(next_block_size)), use_awaitable);
            memcpy(&next_block_size, &data, sizeof(next_block_size));

            if (next_block_size > data.size())
            {
                throw std::runtime_error("Next block size is more then 1024 bytes.");
            }

            co_await boost::asio::async_read(client, buffer(data, next_block_size), use_awaitable);
            protocol.onReceivePackage(next_block_size, data);
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
