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

    class Protocol
    {
    public:
        Protocol()
        {
        }

        void onReceivePackage(Message &inMessage, Message &outMessage)
        {
            char *startPosition = inMessage.data.data();

            std::size_t decision{0};
            memcpy(&decision, startPosition, sizeof(decision));
            startPosition += sizeof(decision);

            // TODO: Add enum for decision
            // 1 -> ping
            if (decision == 1)
            {
                std::size_t request_or_response{0};
                memcpy(&request_or_response, startPosition, sizeof(request_or_response));
                startPosition += sizeof(request_or_response);

                // 1 means it is a request
                if (request_or_response == 1)
                {
                    std::cout << "Received ping request." << std::endl;

                    char *startOutPosition = outMessage.data.data();

                    std::size_t typeOfRequest = 1;
                    std::size_t response = 2;

                    std::size_t totalSize = sizeof(typeOfRequest) + sizeof(response);
                    outMessage.block_size = totalSize;

                    memcpy(startOutPosition, &typeOfRequest, sizeof(typeOfRequest));
                    startOutPosition += sizeof(typeOfRequest);

                    memcpy(startOutPosition, &response, sizeof(response));
                    startOutPosition += sizeof(response);
                }
                else if (request_or_response == 2)
                {
                    std::cout << "Received ping response." << std::endl;
                }
            }
        }

    private:
        //
    };

    awaitable<void> connect_to_server(boost::asio::io_context &ctx, const std::string &host, const std::string &port)
    {
        ////////////////////////connect
        tcp::socket server(ctx);
        auto target = *tcp::resolver(ctx).resolve(host, port);
        co_await server.async_connect(target, use_awaitable);

        ////////////////////////write
        Message outMessage{0};
        char *startOutPosition = outMessage.data.data();

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

        ////////////////////////read
        Message inMessage{0};
        Message anotherOutMessage{0};

        co_await boost::asio::async_read(server, buffer(inMessage.data, sizeof(inMessage.block_size)), use_awaitable);
        memcpy(&inMessage.block_size, &inMessage.data, sizeof(inMessage.block_size));

        if (inMessage.block_size > inMessage.data.size())
        {
            throw std::runtime_error("Block size is more then 1024 bytes.");
        }

        co_await boost::asio::async_read(server, buffer(inMessage.data, inMessage.block_size), use_awaitable);
        Protocol protocol;
        protocol.onReceivePackage(inMessage, anotherOutMessage);
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