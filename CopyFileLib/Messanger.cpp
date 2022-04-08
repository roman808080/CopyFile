#include "pch.h"

#include "Messanger.h"
#include "MessageListenerInterface.h"

void Messanger::subscribe(std::shared_ptr<MessageListenerInterface> listener)
{
	std::unique_lock<std::mutex> listenerLock(listenerMutex);
	listeners.push_back(listener);
}

void Messanger::notifyAboutError()
{
	std::unique_lock<std::mutex> listenerLock(listenerMutex);
	for (const auto& listener: listeners)
	{
		listener->notifyAboutError();
	}
}

