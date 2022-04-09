#pragma once
#include <memory>

#include "ThreadsafeQueue.h"

class OutputFile;

class Writer
{
public:
	Writer(std::shared_ptr<OutputFile> outputFile,
		   std::shared_ptr<ThreadsafeQueue<std::vector<char>>> queue);

	Writer(const Writer&) = delete;
    Writer& operator=(const Writer&) = delete;
    Writer(Writer&&) = delete;
    Writer& operator=(Writer&&) = delete;

	void operator()();
	void write();

private:
	void writeToFile();

private:
	std::shared_ptr<OutputFile> outputFile;
	std::shared_ptr<ThreadsafeQueue<std::vector<char>>> queue;
};

