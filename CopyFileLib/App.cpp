#include "pch.h"
#include "App.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>

#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/scope_exit.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/interprocess/sync/file_lock.hpp>

#include "OutputFile.h"
#include "InputFile.h"

#include "Router.h"
#include "SharedMemory.h"
#include "Constants.h"

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
	file_lock inputFileLock(inputFileName.c_str());
	std::unique_ptr<SharedMemory> sharedMemory(nullptr);

	// trying to create a shared memory
	if (inputFileLock.try_lock())
	{
		std::lock_guard<file_lock> lock(inputFileLock, std::adopt_lock);
		sharedMemory = std::move(SharedMemory::tryCreateSharedMemory(sharedMemoryName));

		shared_memory_buffer* data = sharedMemory->get();
		InputFile inputFile(inputFileName);
		readFromFileToSharedMemory(inputFile, data);

		return;
	}

	OutputFile outputFile(outputFileName);
	file_lock outputFileLock(outputFileName.c_str());

	boost::posix_time::ptime untilTime = boost::posix_time::second_clock::local_time() + boost::posix_time::seconds(Constants::Timeout);
	if (!outputFileLock.timed_lock(untilTime))
	{
		throw std::runtime_error("Failed to acquire lock as a client");
	}

	std::lock_guard<file_lock> lock(outputFileLock, std::adopt_lock);

	sharedMemory = std::move(SharedMemory::attachSharedMemory(sharedMemoryName));
	shared_memory_buffer* data = sharedMemory->get();

	writeFromSharedMemoryToFile(outputFile, data);
}
