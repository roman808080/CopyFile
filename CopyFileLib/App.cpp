#include "pch.h"
#include "App.h"

#include "Messenger.h"

#include "OutputFile.h"
#include "InputFile.h"
#include "FileUtils.h"
#include "FileInfo.h"

#include "ThreadsafeQueue.h"
#include "Reader.h"
#include "Writer.h"

App::App(const std::string& inputFileName, const std::string& outputFileName, const size_t blockSize)
	: inputFileName(inputFileName)
	, outputFileName(outputFileName)
	, blockSize(blockSize)
	, messenger(std::make_shared<Messenger>())
{
}

void App::run()
{
	try
	{
		tryRun();
	}
	catch (const std::exception& exc)
	{
		messenger->notifyAboutError(exc.what());
	}
}

void App::tryRun()
{
	std::shared_ptr<OutputFile> outputFile(std::make_shared<OutputFile>(outputFileName));

    auto fileInfo = std::make_unique<FileInfo>(inputFileName, blockSize);
    auto inputFile = std::make_shared<InputFile>(std::move(fileInfo));

	size_t maxQueueSize = Constants::MaxOccupiedMemory / blockSize;
    std::shared_ptr<ThreadsafeQueue<std::vector<char>>> queue(std::make_shared<ThreadsafeQueue<std::vector<char>>>(maxQueueSize));

    std::shared_ptr<Reader> reader(std::make_shared<Reader>(inputFile, queue));
	reader->setMessenger(messenger);
    std::jthread readThread([reader]() {
			reader->read();
        });

    std::shared_ptr<Writer> writer(std::make_shared<Writer>(outputFile, queue));
	writer->setMessenger(messenger);
    std::jthread writeThread([writer]() {
            writer->write();
        });
}
