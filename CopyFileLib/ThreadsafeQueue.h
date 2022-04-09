#pragma once

#include <memory>
#include <mutex>

template<typename T>
class ThreadsafeQueue
{
private:
	struct Node
	{
		std::unique_ptr<T> data;
		std::unique_ptr<Node> next;
	};

	std::mutex headMutex;
	std::mutex tailMutex;

	std::unique_ptr<Node> head;
	Node* tail;

	std::condition_variable dataCond;
	std::condition_variable fullQueueCond;

	std::atomic<bool> finalized = false;
	std::atomic<unsigned int> size = 0;

	const unsigned int maxQueueSize;

	Node* getTail()
	{
		std::lock_guard<std::mutex> tailLock(tailMutex);
		return tail;
	}

	std::unique_ptr<Node> popHead()
	{
		std::unique_ptr<Node> oldHead = std::move(head);
		head = std::move(oldHead->next);

		--size;
		fullQueueCond.notify_one();

		return oldHead;
	}

	std::unique_ptr<Node> waitPopHead()
	{
		std::unique_lock<std::mutex> headLock(headMutex);
		dataCond.wait(headLock, [&] {return head.get() != getTail(); });

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

	void lockAndPush(std::unique_ptr<T> newData)
	{
		std::unique_lock<std::mutex> tailLock(tailMutex);
		fullQueueCond.wait(tailLock, [&] {return size < maxQueueSize; });

		tail->data = std::move(newData);

		std::unique_ptr<Node> newEmptyNode(new Node);
		Node* const newTail = newEmptyNode.get();

		tail->next = std::move(newEmptyNode);
		tail = newTail;

		++size;
	}

public:
	ThreadsafeQueue(size_t maxSize)
		: head(new Node)
		, tail(head.get())
		, maxQueueSize(maxSize)
	{}

	ThreadsafeQueue(const ThreadsafeQueue&) = delete;
    ThreadsafeQueue& operator=(const ThreadsafeQueue&) = delete;
    ThreadsafeQueue(ThreadsafeQueue&&) = delete;
    ThreadsafeQueue& operator=(ThreadsafeQueue&&) = delete;

	std::unique_ptr<T> waitAndPop()
	{
		std::unique_ptr<Node> const oldHead = waitPopHead();
		return std::move(oldHead->data);
	}

	std::unique_ptr<T> tryPop()
	{
		std::unique_ptr<Node> oldHead = tryPopHead();
		return oldHead ? std::move(oldHead->data) : std::unique_ptr<T>();
	}

	bool empty()
	{
		std::lock_guard<std::mutex> headLock(headMutex);
		return (head.get() == getTail());
	}

	void push(std::unique_ptr<T> newData)
	{
		lockAndPush(std::move(newData));
		dataCond.notify_one();
	}

	void finalize()
	{
		finalized = true;
	}

	bool isFinished()
	{
		return (empty() && finalized);
	}
};

