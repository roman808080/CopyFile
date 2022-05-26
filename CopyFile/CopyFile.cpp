// CopyFile.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
/*#include <iostream>
#include "App.h"
#include "Constants.h"

#include <boost/regex.hpp>

int main(int argc, char* argv[])
{
	try
	{
		if (argc < 3)
		{
			std::cout << "Not enough parameters. Should be: CopyFile <source> <destination>" << std::endl;
			return 1;
		}

		const std::string inputFileName = argv[1];
		const std::string outputFileName = argv[2];

		App app(inputFileName, outputFileName, Constants::Megabyte);
		app.run();
	}
	catch (const std::exception& exc)
	{
		std::cout << "An exception has happened: " << exc.what() << std::endl;
		return 1;
	}

	return 0;
}*/

#include <iostream>

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/this_coro.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/signal_set.hpp>

#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>

#include <cstdio>
#include <boost/bind.hpp>

using boost::asio::awaitable;
using boost::asio::buffer;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
using boost::asio::ip::tcp;
namespace this_coro = boost::asio::this_coro;

boost::asio::io_context io_context(1);

awaitable<void> connect_to_server()
{
	try
	{
		boost::asio::steady_timer theTimer(io_context, boost::asio::chrono::seconds(1));
		co_await theTimer.async_wait(use_awaitable);

		tcp::socket toServer(io_context);
		auto target = *tcp::resolver(io_context).resolve("127.0.0.1", "55555");

		co_await toServer.async_connect(target, use_awaitable);

		std::array<char, 1024> data{'H', 'e', 'l', 'o'};
		for (;;)
		{
			co_await async_write(toServer, buffer(data, 4), use_awaitable);

			std::array<char, 1024> response{0};
			// co_await toServer.async_read_some(buffer(response), use_awaitable);
			co_await boost::asio::async_read(toServer, buffer(response, 4), use_awaitable);

			std::cout << &response[0] << std::endl;

			boost::asio::steady_timer theTimer2(io_context, boost::asio::chrono::seconds(1));
			co_await theTimer2.async_wait(use_awaitable);
		}
	}
	catch (std::exception &e)
	{
		std::printf("echo Exception: %s\n", e.what());
	}
}

awaitable<void> echo(tcp::socket socket)
{

	try
	{
		char data[1024];
		for (;;)
		{
			std::size_t n = co_await socket.async_read_some(boost::asio::buffer(data), use_awaitable);
			co_await async_write(socket, boost::asio::buffer(data, n), use_awaitable);
		}
	}
	catch (std::exception &e)
	{
		std::printf("echo Exception: %s\n", e.what());
	}
}

awaitable<void> listener()
{

	try
	{
		auto executor = co_await this_coro::executor;

		tcp::acceptor acceptor(executor, {tcp::v4(), 55555});
		for (;;)
		{
			tcp::socket socket = co_await acceptor.async_accept(use_awaitable);
			co_spawn(
				executor,
				[socket = std::move(socket)]() mutable
				{
					return echo(std::move(socket));
				},
				detached);
		}
	}
	catch (std::exception &e)
	{
		std::printf("echo Exception: %s\n", e.what());
	}
}

int main()
{
	try
	{

		boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
		signals.async_wait([&](auto, auto)
						   { io_context.stop(); });

		co_spawn(io_context, listener, detached);
		co_spawn(io_context, connect_to_server, detached);

		io_context.run();
	}
	catch (std::exception &e)
	{
		std::printf("Exception: %s\n", e.what());
	}
}
