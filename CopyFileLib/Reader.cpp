#include "pch.h"
#include "Reader.h"

#include "InputFile.h"
#include "Messenger.h"
#include "Router.h"

Reader::Reader(std::shared_ptr<InputFile> inputFile, std::shared_ptr<Router> router)
	: inputFile(inputFile)
	, router(router)
	, errorHappend(false)
	, messenger(nullptr)
{
}

Reader::~Reader()
{
}

void Reader::operator()()
{
	readFromFile();
}

void Reader::read()
{
	readFromFile();
}

void Reader::setMessenger(std::shared_ptr<Messenger> messenger)
{
	this->messenger = messenger;
}

void Reader::notifyAboutError()
{
	errorHappend = true;
}

void Reader::readFromFile()
{
	try
	{
		tryReadFromFile();
	}
	catch (const std::exception& exc)
	{
		notifyMessangerAboutError(exc.what());
	}
}

void Reader::tryReadFromFile()
{    
	std::vector<char>* previousBlock = nullptr;

	while (!inputFile->isFinished() &&
		   !errorHappend)
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

void Reader::notifyMessangerAboutError(const std::string& errorString)
{
	if (messenger.get() != nullptr)
	{
		messenger->notifyAboutError(errorString);
	}
}
