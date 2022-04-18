#include "pch.h"
#include "App.h"

#include "Messenger.h"

#include "OutputFile.h"
#include "InputFile.h"
#include "FileUtils.h"
#include "FileInfo.h"

#include "Reader.h"
#include "Writer.h"
#include "Router.h"

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
	std::shared_ptr<Router> router(std::make_shared<Router>());

    std::shared_ptr<Reader> reader(std::make_shared<Reader>(inputFile, router));
	reader->setMessenger(messenger);
    std::jthread readThread([reader]() {
			reader->read();
        });

    std::shared_ptr<Writer> writer(std::make_shared<Writer>(outputFile, router));
	writer->setMessenger(messenger);
    std::jthread writeThread([writer]() {
            writer->write();
        });
}
