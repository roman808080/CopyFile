#include "pch.h"
#include "App.h"

#include <iostream>
#include <thread>
#include <chrono>

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include "unique_generator.h"

#include "OutputFile.h"
#include "InputFile.h"

#include "anonymous_semaphore_shared_data.h"

#include "Router.h"

using namespace boost::interprocess;

namespace
{
	class RouterGuard
	{
	public:
		RouterGuard(std::shared_ptr<Router> router)
			: router(router)
		{
		}

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
		Chunk previousBlock{nullptr, 0};

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

	void run_server(std::shared_ptr<InputFile> inputFile)
	{
		// Erase previous shared memory
		shared_memory_object::remove("shared_memory");
		shared_memory_object shm(create_only, "shared_memory", read_write);

		// Set size
		shm.truncate(sizeof(shared_memory_buffer));

		// Map the whole shared memory in this process
		mapped_region region(shm, read_write);

		// Get the address of the mapped region
		void *addr = region.get_address();

		// Construct the shared structure in memory
		shared_memory_buffer *data = new (addr) shared_memory_buffer;

		int iteration = 0;
		while (!inputFile->isFinished())
		{
			data->nempty.wait();
			data->mutex.wait();

			auto item = &data->items[iteration % shared_memory_buffer::NumItems];
			inputFile->readBlock(item);

			data->mutex.post();
			data->nstored.post();

			++iteration;
		}

		data->nempty.wait();
		data->mutex.wait();

		data->items[iteration % shared_memory_buffer::NumItems].size = 0;

		data->mutex.post();
		data->nstored.post();


		// Erase shared memory
		shared_memory_object::remove("shared_memory");
	}

	unique_generator<Block*> run_client()
	{
		// Create a shared memory object.
		shared_memory_object shm(open_only, "shared_memory", read_write);

		// Map the whole shared memory in this process
		mapped_region region(shm, read_write);

		// Get the address of the mapped region
		void *addr = region.get_address();

		// Obtain the shared structure
		shared_memory_buffer *data = static_cast<shared_memory_buffer *>(addr);

		// Extract the data
		for (int i = 0;; ++i)
		{
			data->nstored.wait();
			data->mutex.wait();

			auto item = &data->items[i % shared_memory_buffer::NumItems];
			if (item->size == 0)
			{
				co_return;
			}

			co_yield item;

			data->mutex.post();
			data->nempty.post();
		}

		// Erase shared memory
		shared_memory_object::remove("shared_memory");
	}
}

App::App(const size_t blockSize)
	: inputFileName(""), outputFileName(""), method("default"), blockSize(blockSize), isClient(false)
{
}

void App::run()
{
	if (method == "shared")
	{
		copyFileSharedMemoryMethod();
		return;
	}

	copyFileDefaultMethod();
}

void App::setInputFile(const std::string &inputFileName)
{
	this->inputFileName = inputFileName;
}

void App::setOutputFile(const std::string &outputFileName)
{
	this->outputFileName = outputFileName;
}

void App::setMethod(const std::string &method)
{
	this->method = method;
}

void App::setSharedMemoryName(const std::string &sharedMemoryName)
{
	this->sharedMemoryName = sharedMemoryName;
}

void App::setIsClient(bool isClient)
{
	this->isClient = isClient;
}

void App::copyFileDefaultMethod()
{
	std::shared_ptr<OutputFile> outputFile(std::make_shared<OutputFile>(outputFileName));
	auto inputFile = std::make_shared<InputFile>(inputFileName);

	std::shared_ptr<Router> router(std::make_shared<Router>());

	std::jthread readThread(readFromFile, inputFile, router);
	std::jthread writeThread(writeToFile, outputFile, router);
}

void App::copyFileSharedMemoryMethod()
{
	if (isClient)
	{
		std::shared_ptr<OutputFile> outputFile(std::make_shared<OutputFile>(outputFileName));
		for(auto block : run_client())
		{
			outputFile->write(block);
		}
	}
	else
	{
		auto inputFile = std::make_shared<InputFile>(inputFileName);
		run_server(inputFile);
	}
}