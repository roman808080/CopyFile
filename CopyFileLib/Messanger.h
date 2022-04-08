#pragma once
#include <memory>
#include <vector>
#include <mutex>

class MessageListenerInterface;

class Messanger
{
public:
	void subscribe(std::shared_ptr<MessageListenerInterface> listener);
	void notifyAboutError();

private:
	std::vector<std::shared_ptr<MessageListenerInterface>> listeners;
	std::mutex listenerMutex;
};

