#include "pch.h"
#include "Reader.h"

#include <iostream>

#include "InputFile.h"
#include "Router.h"

Reader::Reader(std::shared_ptr<InputFile> inputFile, std::shared_ptr<Router> router)
	: inputFile(inputFile)
	, router(router)
{
}

Reader::~Reader()
{
}

void Reader::read()
{
	try
	{
		tryReadFromFile();
	}
	catch (const std::exception& exc)
	{
		// Ideally should be replaced by a logger. Before partially this role executed Messanger, but I removed it to simplify the code.
		std::cout << "An error has happend: " << exc.what() << std::endl;

		// Stop rotation in case of an error.
		router->stopRotation();
	}
}

void Reader::tryReadFromFile()
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

