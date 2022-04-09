#include "pch.h"
#include "Writer.h"

#include "OutputFile.h"
#include "Messanger.h"

Writer::Writer(std::shared_ptr<OutputFile> outputFile,
			   std::shared_ptr<ThreadsafeQueue<std::vector<char>>> queue)
	: outputFile(outputFile)
	, queue(queue)
	, errorHappend(false)
	, messenger(nullptr)
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
	while (!queue->isFinished() &&
		   !errorHappend)
	{
		auto block = std::move(queue->waitAndPop());
		outputFile->write(std::move(block));
	}
}

void Writer::notifyMessangerAboutError(const std::string& errorString)
{
	if (messenger.get() != nullptr)
	{
		messenger->notifyAboutError(errorString);
	}
}
