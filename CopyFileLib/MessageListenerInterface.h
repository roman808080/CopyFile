#pragma once
class MessageListenerInterface
{
public:
	virtual void notifyAboutError() = 0;
	virtual ~MessageListenerInterface(){}
};

