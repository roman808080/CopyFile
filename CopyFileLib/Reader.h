#pragma once

#include <memory>
#include <vector>
#include <atomic>

#include "ThreadsafeQueue.h"
#include "MessageListenerInterface.h"

class InputFile;

class Reader: public MessageListenerInterface
{
public:
	Reader(std::shared_ptr<InputFile> inputFile,
               std::shared_ptr<ThreadsafeQueue<std::vector<char>>> queue);

	Reader(const Reader&) = delete;
    Reader& operator=(const Reader&) = delete;
    Reader(Reader&&) = delete;
    Reader& operator=(Reader&&) = delete;

	void operator()();
	void read();

	virtual void notifyAboutError() override;

private:
	void readFromFile();
	void tryReadFromFile();

private:
	std::shared_ptr<InputFile> inputFile;
	std::shared_ptr<ThreadsafeQueue<std::vector<char>>> queue;

	std::atomic<bool> errorHappend = false;
};

