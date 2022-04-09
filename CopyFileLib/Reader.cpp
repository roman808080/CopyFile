#include "pch.h"
#include "Reader.h"

#include <iostream>
#include "InputFile.h"

Reader::Reader(std::shared_ptr<InputFile> inputFile, std::shared_ptr<ThreadsafeQueue<std::vector<char>>> queue)
	: inputFile(inputFile)
	, queue(queue)
	, errorHappend(false)
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
		std::cout << "An error has happend: " << exc.what() << std::endl;
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
