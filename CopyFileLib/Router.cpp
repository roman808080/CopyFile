#include "pch.h"
#include "Router.h"

#include <thread>
#include "Constants.h"

namespace
{
	const size_t kMaxElementSize = 15;
}

Router::Router()
	: cache(kMaxElementSize * Constants::Kilobyte)
{
	for (int startPosition = 0; startPosition < kMaxElementSize * Constants::Kilobyte; startPosition += Constants::Kilobyte)
	{
		Chunk chunk {&cache[startPosition], Constants::Kilobyte};
		usedOutputBlocks.push_back(chunk);
	}
}

Router::~Router()
{
}


Chunk Router::rotateInputBlocks(Chunk readyBlock)
{
	Chunk newBlock{nullptr, 0};
	while ((newBlock = tryRotateInputBlocks(readyBlock)).startPosition == nullptr)
	{
		if (stopped.load())
		{
			return newBlock;
		}

		std::this_thread::yield();
	}

	return newBlock;
}

Chunk Router::rotateOutputBlocks(Chunk usedBlock)
{	
	Chunk newBlock{nullptr, 0};
	while ((newBlock = tryRotateOutputBlocks(usedBlock)).startPosition == nullptr)
	{
		if (stopped.load())
		{
			return newBlock;
		}

		std::this_thread::yield();
	}

	return newBlock;
}

void Router::stopRotation()
{
	stopped = true;
}

bool Router::isRotationStopped()
{
	return stopped;
}

Chunk Router::tryRotateInputBlocks(Chunk readyBlock)
{
	std::unique_lock<std::mutex> lock(criticalSection);

	if (usedOutputBlocks.empty())
	{
		Chunk chunk {nullptr, 0};
		return chunk;
	}

	if (readyBlock.startPosition != nullptr)
	{
		readyOutputBlocks.push_back(readyBlock);
	}

	Chunk newBlock = usedOutputBlocks.front();
	usedOutputBlocks.pop_front();

	return newBlock;
}

Chunk Router::tryRotateOutputBlocks(Chunk usedBlock)
{
	std::unique_lock<std::mutex> lock(criticalSection);

	if (readyOutputBlocks.empty())
	{
		Chunk chunk{ nullptr, 0 };
		return chunk;
	}

	if (usedBlock.startPosition != nullptr)
	{
		usedOutputBlocks.push_back(usedBlock);
	}

	Chunk newBlock = readyOutputBlocks.front();
	readyOutputBlocks.pop_front();

	return newBlock;
}

