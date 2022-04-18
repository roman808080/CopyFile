#include "pch.h"
#include "App.h"

#include <iostream>

#include "OutputFile.h"
#include "InputFile.h"

#include "Reader.h"
#include "Writer.h"
#include "Router.h"

App::App(const std::string& inputFileName, const std::string& outputFileName, const size_t blockSize)
	: inputFileName(inputFileName)
	, outputFileName(outputFileName)
	, blockSize(blockSize)
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
		// Ideally should be replaced by a logger. Before partially this role executed Messanger, but I removed it to simplify the code.
		std::cout << "An error has happend: " << exc.what() << std::endl;
	}
}

void App::tryRun()
{
	std::shared_ptr<OutputFile> outputFile(std::make_shared<OutputFile>(outputFileName));
    auto inputFile = std::make_shared<InputFile>(inputFileName);

	std::shared_ptr<Router> router(std::make_shared<Router>());

    std::shared_ptr<Reader> reader(std::make_shared<Reader>(inputFile, router));
    std::jthread readThread([reader]() {
			reader->read();
        });

    std::shared_ptr<Writer> writer(std::make_shared<Writer>(outputFile, router));
    std::jthread writeThread([writer]() {
            writer->write();
        });
}
