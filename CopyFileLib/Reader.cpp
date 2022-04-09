#include "pch.h"
#include "Reader.h"

#include "InputFile.h"
#include "Messenger.h"

Reader::Reader(std::shared_ptr<InputFile> inputFile, std::shared_ptr<ThreadsafeQueue<std::vector<char>>> queue)
	: inputFile(inputFile)
	, queue(queue)
	, errorHappend(false)
	, messenger(nullptr)
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
	while (!inputFile->isFinished() &&
		   !errorHappend)
	{
		auto block = std::move(inputFile->readBlock());
		queue->push(std::move(block));
	}

	queue->finalize();
}

void Reader::notifyMessangerAboutError(const std::string& errorString)
{
	if (messenger.get() != nullptr)
	{
		messenger->notifyAboutError(errorString);
	}
}
