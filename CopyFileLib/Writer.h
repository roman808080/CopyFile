#pragma once
#include <memory>

#include "ThreadsafeQueue.h"
#include "MessageListenerInterface.h"

class OutputFile;

class Writer: MessageListenerInterface
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

	virtual void notifyAboutError() override;

private:
	void writeToFile();
	void tryWriteToFile();

private:
	std::shared_ptr<OutputFile> outputFile;
	std::shared_ptr<ThreadsafeQueue<std::vector<char>>> queue;

	std::atomic<bool> errorHappend = false;
};

