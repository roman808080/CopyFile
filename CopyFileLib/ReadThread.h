#pragma once

#include <memory>
#include <vector>

#include "ThreadsafeQueue.h"

class InputFile;


class ReadThread
{
public:
	ReadThread(std::shared_ptr<InputFile> inputFile,
               std::shared_ptr<ThreadsafeQueue<std::vector<char>>> queue);

	void operator()();

private:
	void readFromFile();

private:
	std::shared_ptr<InputFile> inputFile;
	std::shared_ptr<ThreadsafeQueue<std::vector<char>>> queue;
};

