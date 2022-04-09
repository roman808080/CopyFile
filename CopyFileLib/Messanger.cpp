#include "pch.h"

#include <iostream>

#include "Messanger.h"
#include "MessageListenerInterface.h"

void Messanger::subscribe(std::shared_ptr<MessageListenerInterface> listener)
{
	std::unique_lock<std::mutex> listenerLock(listenerMutex);
	listeners.push_back(listener);
}

void Messanger::notifyAboutError(const std::string& errorString)
{
	std::unique_lock<std::mutex> listenerLock(listenerMutex);

	std::cout << "An error has happend: " << errorString << std::endl;
	for (const auto& listener: listeners)
	{
		listener->notifyAboutError();
	}
}

