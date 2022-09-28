#include "pch.h"
#include "App.h"

#include <iostream>
#include <thread>

#include "OutputFile.h"
#include "InputFile.h"

#include "Router.h"

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
}

App::App(const size_t blockSize)
	: inputFileName("")
	, outputFileName("")
	, method ("default")
	, blockSize(blockSize)
	, isClient(false)
{
}

void App::run()
{
	std::shared_ptr<OutputFile> outputFile(std::make_shared<OutputFile>(outputFileName));
    auto inputFile = std::make_shared<InputFile>(inputFileName);

	std::shared_ptr<Router> router(std::make_shared<Router>());

    std::jthread readThread(readFromFile, inputFile, router);
    std::jthread writeThread(writeToFile, outputFile, router);
}

void App::setInputFile(const std::string& inputFileName)
{
	this->inputFileName = inputFileName;
}

void App::setOutputFile(const std::string& outputFileName)
{
	this->outputFileName = outputFileName;
}

void App::setMethod(const std::string& method)
{
	this->method = method;
}

void App::setSharedMemoryName(const std::string& sharedMemoryName)
{
	this->sharedMemoryName = sharedMemoryName;
}

void App::setIsClient(bool isClient)
{
	this->isClient = isClient;
}