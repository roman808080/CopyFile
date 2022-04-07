#pragma once

#include <memory>
#include <mutex>

template<typename T>
class ThreadsafeQueue
{
private:
	struct Node
	{
		std::shared_ptr<T> data;
		std::unique_ptr<Node> next;
	};

	std::mutex headMutex;
	std::mutex tailMutex;

	std::unique_ptr<Node> head;
	Node* tail;

	std::condition_variable dataCond;

	Node* getTail()
	{
		std::lock_guard<std::mutex> tailLock(tailMutex);
		return tail;
	}

	std::unique_ptr<Node> popHead()
	{
		std::unique_ptr<Node> oldHead = std::move(head);
		head = std::move(oldHead->next);
		return oldHead;
	}

	std::unique_lock<std::mutex> waitForData()
	{
		std::unique_lock<std::mutex> headLock(headMutex);
		dataCond.wait(headLock, [&] {return head.get() != getTail(); });
		return std::move(headLock);
	}

	std::unique_ptr<Node> waitPopHead()
	{
		std::unique_lock<std::mutex> headLock(waitForData());
		return popHead();
	}

	std::unique_ptr<Node> waitPopHead(T& value)
	{
		std::unique_lock<std::mutex> headLock(waitForData());
		value = std::move(*head->data);
		return popHead();
	}

	std::unique_ptr<Node> tryPopHead()
	{
		std::lock_guard<std::mutex> headLock(headMutex);
		if (head.get() == getTail())
		{
			return std::unique_ptr<Node>();
		}

		return popHead();
	}

	std::unique_ptr<Node> tryPopHead(T& value)
	{
		std::lock_guard<std::mutex> headLock(headMutex);
		if (head.get() == getTail())
		{
			return std::unique_ptr<Node>();
		}

		value = std::move(*head->data);
		return popHead();
	}

public:
	ThreadsafeQueue()
		: head(new Node)
		, tail(head.get())
	{}

	ThreadsafeQueue(const ThreadsafeQueue& other) = delete;
	ThreadsafeQueue& operator=(const ThreadsafeQueue& other) = delete;

	std::shared_ptr<T> waitAndPop()
	{
		std::unique_ptr<Node> const oldHead = waitPopHead();
		return oldHead->data;
	}

	void waitAndPop(T& value)
	{
		std::unique_ptr<Node> const oldHead = waitPopHead(value);
	}

	std::shared_ptr<T> tryPop()
	{
		std::unique_ptr<Node> oldHead = tryPopHead();
		return oldHead ? oldHead->data : std::shared_ptr<T>();
	}

	bool tryPop(T& value)
	{
		std::unique_ptr<Node> const oldHead = tryPopHead(value);
		return oldHead.get() != nullptr;
	}

	bool empty()
	{
		std::lock_guard<std::mutex> headLock(headMutex);
		return (head.get() == getTail());
	}

	void push(T newValue)
	{
		std::shared_ptr<T> newData(std::make_shared<T>(std::move(newValue)));
		std::unique_ptr<Node> newEmptyNode(new Node);
		{
			std::lock_guard<std::mutex> tailLock(tailMutex);

			tail->data = newData;
			Node* const newTail = newEmptyNode.get();

			tail->next = std::move(newEmptyNode);
			tail = newTail;
		}
	}

};

