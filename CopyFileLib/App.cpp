#include "pch.h"
#include "App.h"

#include <iostream>

#include "OutputFile.h"
#include "InputFile.h"

#include "Writer.h"
#include "Router.h"

namespace
{
    void readFromFile(std::shared_ptr<InputFile> inputFile,
					  std::shared_ptr<Router> router)
    {
		std::vector<char>* previousBlock = nullptr;

		while (!inputFile->isFinished())
		{
			std::vector<char>* currentBlock = router->rotateInputBlocks(previousBlock);
			inputFile->readBlock(currentBlock);
			previousBlock = currentBlock;
		}

		if (previousBlock != nullptr)
		{
			router->rotateInputBlocks(previousBlock);
		}

		router->stopRotation();
    }
}

App::App(const std::string& inputFileName, const std::string& outputFileName, const size_t blockSize)
	: inputFileName(inputFileName)
	, outputFileName(outputFileName)
	, blockSize(blockSize)
{
}

void App::run()
{
	std::shared_ptr<OutputFile> outputFile(std::make_shared<OutputFile>(outputFileName));
    auto inputFile = std::make_shared<InputFile>(inputFileName);

	std::shared_ptr<Router> router(std::make_shared<Router>());

    std::jthread readThread(readFromFile, inputFile, router);

    std::shared_ptr<Writer> writer(std::make_shared<Writer>(outputFile, router));
    std::jthread writeThread([writer]() {
            writer->write();
        });
}

