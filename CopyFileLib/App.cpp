#include "pch.h"
#include "App.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>

#include "OutputFile.h"
#include "InputFile.h"

#include "Router.h"
#include "SharedMemory.h"

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

	void readFromFileToSharedMemory(InputFile& inputFile, shared_memory_buffer* data)
	{
		int iteration = 0;
		while (!inputFile.isFinished())
		{
			data->nempty.wait();

			iteration = iteration % shared_memory_buffer::NumItems;
			auto item = &data->items[iteration];
			inputFile.readBlock(item);

			data->nstored.post();

			++iteration;
		}

		data->nempty.wait();
		data->items[iteration % shared_memory_buffer::NumItems].size = 0;
		data->nstored.post();
	}

	void writeFromSharedMemoryToFile(OutputFile& outputFile, shared_memory_buffer* data)
	{
		// Extract the data
		int iteration = 0;
		while (true)
		{
			data->nstored.wait();

			iteration = iteration % shared_memory_buffer::NumItems;
			auto item = &data->items[iteration];
			if (item->size == 0)
			{
				return;
			}

			outputFile.write(item);

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
	named_mutex namedMutex{open_or_create, sharedMemoryName.c_str()};
	BOOST_SCOPE_EXIT(&sharedMemoryName)
	{
		named_mutex::remove(sharedMemoryName.c_str());
	} BOOST_SCOPE_EXIT_END

	std::unique_ptr<SharedMemory> sharedMemory(nullptr);

	// trying to create a shared memory
	if (namedMutex.try_lock())
	{
		std::lock_guard<named_mutex> lock(namedMutex, std::adopt_lock);
		sharedMemory = std::move(SharedMemory::tryCreateSharedMemory(sharedMemoryName));
	}

	// Check whether the lock and creation was successful
	if (sharedMemory)
	{
		shared_memory_buffer* data = sharedMemory->get();
		InputFile inputFile(inputFileName);
		readFromFileToSharedMemory(inputFile, data);

		return;
	}

	std::lock_guard<named_mutex> lock(namedMutex);
	OutputFile outputFile(outputFileName);

	sharedMemory = std::move(SharedMemory::attachSharedMemory(sharedMemoryName));
	shared_memory_buffer* data = sharedMemory->get();

	writeFromSharedMemoryToFile(outputFile, data);
}
