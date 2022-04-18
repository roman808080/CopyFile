#include "pch.h"
#include "Writer.h"

#include <iostream>

#include "OutputFile.h"
#include "Router.h"

Writer::Writer(std::shared_ptr<OutputFile> outputFile,
			   std::shared_ptr<Router> router)
	: outputFile(outputFile)
	, router(router)
{
}

Writer::~Writer()
{
}

void Writer::write()
{
	try
	{
		tryWriteToFile();
	}
	catch (const std::exception& exc)
	{
		// Ideally should be replaced by a logger. Before partially this role executed Messanger, but I removed it to simplify the code.
		std::cout << "An error has happend: " << exc.what() << std::endl;

		// Stop rotation in case of an error.
		router->stopRotation();
	}
}

void Writer::tryWriteToFile()
{
	std::vector<char>* previousBlock = nullptr;

	bool isFinished = router->isRotationStopped() && previousBlock == nullptr;
	while (!isFinished)
	{
		std::vector<char>* currentBlock = router->rotateOutputBlocks(previousBlock);
		if (currentBlock != nullptr)
		{
			outputFile->write(currentBlock);
		}

		previousBlock = currentBlock;
		isFinished = router->isRotationStopped() && previousBlock == nullptr;
	}
}

