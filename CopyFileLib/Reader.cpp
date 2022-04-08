#include "pch.h"
#include "Reader.h"

#include "InputFile.h"

Reader::Reader(std::shared_ptr<InputFile> inputFile, std::shared_ptr<ThreadsafeQueue<std::vector<char>>> queue)
	: inputFile(inputFile)
	, queue(queue)
{
}

void Reader::operator()()
{
	readFromFile();
}

void Reader::readFromFile()
{    
	while (!inputFile->isFinished())
	{
		auto block = std::move(inputFile->readBlock());
		queue->push(std::move(block));
	}

	queue->finalize();
}
