#include "pch.h"
#include "App.h"

#include <iostream>
#include <filesystem>
#include <fstream>

#include <boost/asio.hpp>
#include <boost/asio/read.hpp>

using boost::asio::awaitable;
using boost::asio::buffer;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::ip::tcp;
using boost::asio::use_awaitable;

#include "OutputFile.h"
#include "InputFile.h"

#include "Router.h"
#include "Constants.h"

namespace
{
	class RouterGuard
	{
	public:
		RouterGuard(std::shared_ptr<Router> router)
			: router(router)
		{}

		~RouterGuard()
		{
			router->stopRotation();
		}

	private:
		std::shared_ptr<Router> router;
	};

    void readFromFile(std::shared_ptr<InputFile> inputFile,
					  std::shared_ptr<Router> router)
    {
		RouterGuard routerGuard(router);
		Chunk previousBlock{nullptr, 0};

		while (!inputFile->isFinished())
		{
			Chunk currentBlock = router->rotateInputBlocks(previousBlock);
			inputFile->readBlock(currentBlock);
			previousBlock = currentBlock;
		}

		if (previousBlock.startPosition != nullptr)
		{
			router->rotateInputBlocks(previousBlock);
		}
    }

	void writeToFile(std::shared_ptr<OutputFile> outputFile,
				     std::shared_ptr<Router> router)
	{
		RouterGuard routerGuard(router);
		Chunk previousBlock{ nullptr, 0};

		bool isFinished = false;
		do
		{
			const Chunk currentBlock = router->rotateOutputBlocks(previousBlock);
			if (currentBlock.startPosition != nullptr)
			{
				outputFile->write(currentBlock);
			}

			previousBlock = currentBlock;
			isFinished = router->isRotationStopped() && previousBlock.startPosition == nullptr;
		} while (!isFinished);
	}

	awaitable<void> writeToSocket(boost::asio::io_context& ctx,
								  const std::string& path_to_file,
								  const std::string& host,
								  const std::string& port,
								  std::shared_ptr<Router> router)
	{
		RouterGuard routerGuard(router);
		Chunk previousBlock{ nullptr, 0};

		tcp::socket to_server(ctx);
		auto target = *tcp::resolver(ctx).resolve(host, port);
        std::size_t path_size = path_to_file.size();

		co_await to_server.async_connect(target, use_awaitable);
        std::array<char, Constants::Kilobyte> data{ 0 };

        memcpy(&data, &path_size, sizeof(path_size));
		co_await async_write(to_server, buffer(data, sizeof(path_size)), use_awaitable);
		co_await async_write(to_server, buffer(path_to_file, path_to_file.size()), use_awaitable);

        std::size_t file_size = std::filesystem::file_size(path_to_file);
        memcpy(&data, &file_size, sizeof(path_size));
		co_await async_write(to_server, buffer(data, sizeof(file_size)), use_awaitable);

		bool isFinished = false;
		do
		{
			const Chunk currentBlock = router->rotateOutputBlocks(previousBlock);
			if (currentBlock.startPosition != nullptr)
			{
				co_await async_write(to_server, buffer(currentBlock.startPosition, currentBlock.size), use_awaitable);
			}

			previousBlock = currentBlock;
			isFinished = router->isRotationStopped() && previousBlock.startPosition == nullptr;
		} while (!isFinished);
	}
}

App::App(const std::string& inputFileName, const std::string& outputFileName, const size_t blockSize)
	: inputFileName(inputFileName)
	, outputFileName(outputFileName)
	, blockSize(blockSize)
{
}

void App::setRemote(const std::string& host, const std::string& port)
{
	this->host = host;
	this->port = port;
}

void App::run()
{
	std::shared_ptr<OutputFile> outputFile(std::make_shared<OutputFile>(outputFileName));
    auto inputFile = std::make_shared<InputFile>(inputFileName);

	std::shared_ptr<Router> router(std::make_shared<Router>());

	std::jthread writeThread([&] {
			if (port == "") {
				writeToFile(outputFile, router);
				return;
			}

			boost::asio::io_context ctx;
			co_spawn(ctx, writeToSocket(ctx, this->inputFileName, this->host, this->port, router), detached);
			ctx.run();
		});

    std::jthread readThread(readFromFile, inputFile, router);
}
