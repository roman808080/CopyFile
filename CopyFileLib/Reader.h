#pragma once

#include <memory>
#include <vector>
#include <atomic>
#include <string>

#include "MessageListenerInterface.h"

class InputFile;
class Messenger;
class Router;

class Reader: public MessageListenerInterface
{
public:
	Reader(std::shared_ptr<InputFile> inputFile,
           std::shared_ptr<Router> router);

	~Reader();

	Reader(const Reader&) = delete;
    Reader& operator=(const Reader&) = delete;
    Reader(Reader&&) = delete;
    Reader& operator=(Reader&&) = delete;

	void read();

	void setMessenger(std::shared_ptr<Messenger> messenger);
	virtual void notifyAboutError() override;

private:
	void tryReadFromFile();

	void notifyMessangerAboutError(const std::string& errorString);

private:
	std::shared_ptr<InputFile> inputFile;
	std::shared_ptr<Router> router;

	std::atomic<bool> errorHappend = false;
	std::shared_ptr<Messenger> messenger;
};

