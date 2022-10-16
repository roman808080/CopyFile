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

#include <cryptopp/cryptlib.h>
#include <cryptopp/blake2.h>
#include <cryptopp/files.h>
#include <cryptopp/hex.h>

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

	std::string getHash(const std::string& combinedPath)
	{
		CryptoPP::BLAKE2b hash;
		hash.Update(reinterpret_cast<const CryptoPP::byte*>(combinedPath.data()), combinedPath.size());

		std::string digest;
		digest.resize(hash.DigestSize());
		hash.Final(reinterpret_cast<CryptoPP::byte*>(&digest[0]));

		std::string stringHash;
		CryptoPP::StringSource(digest, true, new CryptoPP::HexEncoder(new CryptoPP::StringSink(stringHash)));

		return stringHash;
	}

}

App::App(const size_t blockSize)
	: inputFileName(""), outputFileName(""), method("default"), blockSize(blockSize)
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

	const std::string sharedMemoryName(getHash(inputFileName + outputFileName));

	// try to lock as a source process.
	if (inputFileLock.try_lock())
	{
		std::lock_guard<file_lock> lock(inputFileLock, std::adopt_lock);
		sharedMemory = std::move(SharedMemory::tryCreateSharedMemory(sharedMemoryName));

		shared_memory_buffer* data = sharedMemory->get();
		InputFile inputFile(inputFileName);
		readFromFileToSharedMemory(inputFile, data);

		return;
	}

	// try to lock as a destination process.
	OutputFile outputFile(outputFileName);
	file_lock outputFileLock(outputFileName.c_str());

	if (!outputFileLock.try_lock())
	{
		throw std::runtime_error("Failed to acquire lock as a client. Possibly, there is another client.");
	}

	std::lock_guard<file_lock> lock(outputFileLock, std::adopt_lock);

	sharedMemory = std::move(SharedMemory::attachSharedMemory(sharedMemoryName));
	shared_memory_buffer* data = sharedMemory->get();

	writeFromSharedMemoryToFile(outputFile, data);
}
