#pragma once
#include <memory>

#include "ThreadsafeQueue.h"
#include "MessageListenerInterface.h"

class OutputFile;
class Messenger;
class Router;

class Writer: MessageListenerInterface
{
public:
	Writer(std::shared_ptr<OutputFile> outputFile,
		   std::shared_ptr<Router> router);
	~Writer();

	Writer(const Writer&) = delete;
    Writer& operator=(const Writer&) = delete;
    Writer(Writer&&) = delete;
    Writer& operator=(Writer&&) = delete;

	void operator()();
	void write();

	void setMessenger(std::shared_ptr<Messenger> messenger);
	virtual void notifyAboutError() override;

private:
	void writeToFile();
	void tryWriteToFile();

	void notifyMessangerAboutError(const std::string& errorString);

private:
	std::shared_ptr<OutputFile> outputFile;
	std::shared_ptr<Router> router;

	std::atomic<bool> errorHappend = false;
	std::shared_ptr<Messenger> messenger;
};

