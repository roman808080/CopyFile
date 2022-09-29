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

	template <typename T>
	unique_generator<T> range(T fromInclusive, T toExclusive)
	{
		for (T v = fromInclusive; v < toExclusive; ++v)
		{
			co_yield v;
		}
	}

	void run_server()
	{
		// Erase previous shared memory
		shared_memory_object::remove("shared_memory");

		// Create a shared memory object.
		shared_memory_object shm(create_only // only create
								 ,
								 "shared_memory" // name
								 ,
								 read_write // read-write mode
		);

		// Set size
		shm.truncate(sizeof(shared_memory_buffer));

		// Map the whole shared memory in this process
		mapped_region region(shm // What to map
							 ,
							 read_write // Map it as read-write
		);

		// Get the address of the mapped region
		void *addr = region.get_address();

		// Construct the shared structure in memory
		shared_memory_buffer *data = new (addr) shared_memory_buffer;

		const int NumMsg = 100;

		// Insert data in the array
		for (int i = 0; i < NumMsg; ++i)
		{
			data->nempty.wait();
			data->mutex.wait();
			data->items[i % shared_memory_buffer::NumItems] = i;
			data->mutex.post();
			data->nstored.post();
		}

		// Erase shared memory
		shared_memory_object::remove("shared_memory");
	}

	unique_generator<int> run_client()
	{
		// Create a shared memory object.
		shared_memory_object shm(open_only // only create
								 ,
								 "shared_memory" // name
								 ,
								 read_write // read-write mode
		);

		// Map the whole shared memory in this process
		mapped_region region(shm // What to map
							 ,
							 read_write // Map it as read-write
		);

		// Get the address of the mapped region
		void *addr = region.get_address();

		// Obtain the shared structure
		shared_memory_buffer *data = static_cast<shared_memory_buffer *>(addr);

		const int NumMsg = 100;

		// Extract the data
		for (int i = 0; i < NumMsg; ++i)
		{
			data->nstored.wait();
			data->mutex.wait();

			co_yield data->items[i % shared_memory_buffer::NumItems];

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
		std::cout << "Client: ";
		for(auto val : run_client())
		{
			std::cout << val << " ";
		}
	}
	else
	{
		run_server();
	}
}