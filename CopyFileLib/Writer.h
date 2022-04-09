#pragma once
#include <memory>

#include "ThreadsafeQueue.h"
#include "MessageListenerInterface.h"

class OutputFile;
class Messanger;

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

	void setMessenger(std::shared_ptr<Messanger> messanger);
	virtual void notifyAboutError() override;

private:
	void writeToFile();
	void tryWriteToFile();

	void notifyMessangerAboutError(const std::string& errorString);

private:
	std::shared_ptr<OutputFile> outputFile;
	std::shared_ptr<ThreadsafeQueue<std::vector<char>>> queue;

	std::atomic<bool> errorHappend = false;
	std::shared_ptr<Messanger> messanger;
};

