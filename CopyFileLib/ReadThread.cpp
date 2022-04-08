#include "pch.h"
#include "ReadThread.h"

#include "InputFile.h"

ReadThread::ReadThread(std::shared_ptr<InputFile> inputFile, std::shared_ptr<ThreadsafeQueue<std::vector<char>>> queue)
	: inputFile(inputFile)
	, queue(queue)
{
}

void ReadThread::operator()()
{
	readFromFile();
}

void ReadThread::readFromFile()
{    
	while (!inputFile->isFinished())
	{
		auto block = std::move(inputFile->readBlock());
		queue->push(std::move(block));
	}

	queue->finalize();
}
