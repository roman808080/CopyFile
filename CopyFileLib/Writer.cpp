#include "pch.h"
#include "Writer.h"

#include "OutputFile.h"
#include "Messenger.h"
#include "Router.h"

Writer::Writer(std::shared_ptr<OutputFile> outputFile,
			   std::shared_ptr<Router> router)
	: outputFile(outputFile)
	, router(router)
	, errorHappend(false)
	, messenger(nullptr)
{
}

Writer::~Writer()
{
}

void Writer::operator()()
{
	writeToFile();
}

void Writer::write()
{
	writeToFile();
}

void Writer::setMessenger(std::shared_ptr<Messenger> meseanger)
{
	this->messenger = messenger;
}

void Writer::notifyAboutError()
{
	errorHappend = true;
}

void Writer::writeToFile()
{
	try
	{
		tryWriteToFile();
	}
	catch (const std::exception& exc)
	{
		notifyMessangerAboutError(exc.what());
	}
}

void Writer::tryWriteToFile()
{
	std::vector<char>* previousBlock = nullptr;

	bool isFinished = router->isRotationStopped() && previousBlock == nullptr;
	while (!isFinished && !errorHappend)
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

void Writer::notifyMessangerAboutError(const std::string& errorString)
{
	if (messenger.get() != nullptr)
	{
		messenger->notifyAboutError(errorString);
	}
}
