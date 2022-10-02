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

	class SharedMemory
	{
	public:
		static SharedMemory createSharedMemory(const std::string &sharedMemoryName)
		{
			// Erase previous shared memory
			shared_memory_object::remove(sharedMemoryName.c_str());
			shared_memory_object shm(create_only, sharedMemoryName.c_str(), read_write);

			SharedMemory sharedMemory(SharedMemory(sharedMemoryName, std::move(shm)));


			sharedMemory.initMemoryBuffer();
			return std::move(sharedMemory);
		}

		static SharedMemory attachSharedMemory(const std::string &sharedMemoryName)
		{
			// Create a shared memory object.
			shared_memory_object shm(open_only, sharedMemoryName.c_str(), read_write);

			SharedMemory sharedMemory(SharedMemory(sharedMemoryName, std::move(shm)));

			sharedMemory.castMemoryBuffer();
			return std::move(sharedMemory);
		}

		SharedMemory(const std::string &sharedMemoryName, shared_memory_object &&shm)
		: sharedMemoryName(sharedMemoryName)
		, shm(std::move(shm)) {
			this->shm.truncate(sizeof(shared_memory_buffer));
			this->region = std::move(mapped_region(this->shm, read_write));
		}

		SharedMemory(const SharedMemory&) = delete;
		SharedMemory& operator=(const SharedMemory&) = delete;

		SharedMemory(SharedMemory&& other)
		: sharedMemoryName(std::move(other.sharedMemoryName))
		, shm(std::move(other.shm))
		, region(std::move(other.region))
		, data{other.data}
		{
			other.data = nullptr;
		}

		SharedMemory& operator=(SharedMemory&& other)
		{
			if (&other == this)
			{
				return *this;
			}

			this->sharedMemoryName = std::move(other.sharedMemoryName);
			this->shm = std::move(other.shm);
			this->region = std::move(other.region);

			this->data = other.data;
			other.data = nullptr;

			return *this;
		}

		~SharedMemory()
		{
			if (sharedMemoryName != "")
			{
				shared_memory_object::remove(this->sharedMemoryName.c_str());
			}
		}

		shared_memory_buffer* get()
		{
			return data;
		}

	private:
		void initMemoryBuffer()
		{
			// Get the address of the mapped region
			void* addr = region.get_address();

			// Construct the shared structure in memory
			data = new (addr) shared_memory_buffer; 
		}

		void castMemoryBuffer()
		{
			// Get the address of the mapped region
			void* addr = region.get_address();

			// Obtain the shared structure
			data = static_cast<shared_memory_buffer *>(addr);
		}

	private:
		std::string sharedMemoryName;
		shared_memory_object shm;
		mapped_region region;
		shared_memory_buffer* data;
	};

	void run_server(const std::string& sharedMemoryName, std::shared_ptr<InputFile> inputFile)
	{
		SharedMemory sharedMemory(SharedMemory::createSharedMemory(sharedMemoryName));
		shared_memory_buffer* data = sharedMemory.get();

		int iteration = 0;
		while (!inputFile->isFinished())
		{
			data->nempty.wait();

			iteration = iteration % shared_memory_buffer::NumItems;
			auto item = &data->items[iteration];
			inputFile->readBlock(item);

			data->nstored.post();

			++iteration;
		}

		data->nempty.wait();
		data->items[iteration % shared_memory_buffer::NumItems].size = 0;
		data->nstored.post();
	}

	unique_generator<Block *> run_client(const std::string& sharedMemoryName)
	{
		SharedMemory sharedMemory(SharedMemory::attachSharedMemory(sharedMemoryName));
		shared_memory_buffer* data = sharedMemory.get();

		// Extract the data
		int iteration = 0;
		while (true)
		{
			data->nstored.wait();

			iteration = iteration % shared_memory_buffer::NumItems;
			auto item = &data->items[iteration];
			if (item->size == 0)
			{
				co_return;
			}

			co_yield item;

			data->nempty.post();
			++iteration;
		}
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
		for (auto block : run_client(sharedMemoryName))
		{
			outputFile->write(block);
		}
	}
	else
	{
		auto inputFile = std::make_shared<InputFile>(inputFileName);
		run_server(sharedMemoryName, inputFile);
	}
}