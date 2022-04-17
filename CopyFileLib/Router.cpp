#include "pch.h"
#include "Router.h"

#include <thread>

namespace
{
	const size_t kMaxElementSize = 15;
}

Router::Router()
	: cache(kMaxElementSize)
{
	for (int i = 0; i < cache.size(); ++i)
	{
		usedBlocks.push_back(&cache[i]);
	}
}

std::vector<char>* Router::rotateUsedBlocks(std::vector<char>* usedBlock)
{	
	std::vector<char>* newBlock = nullptr;
	while ((newBlock = tryRotateUsedBlocks(usedBlock)) == nullptr)
	{
		std::this_thread::yield();
	}

	return newBlock;
}

std::vector<char>* Router::rotateReadyBlocks(std::vector<char>* readyBlock)
{
	std::vector<char>* newBlock = nullptr;
	while ((newBlock = tryRotateReadyBlocks(readyBlock)) == nullptr)
	{
		std::this_thread::yield();
	}

	return newBlock;
}

std::vector<char>* Router::tryRotateUsedBlocks(std::vector<char>* usedBlock)
{
	std::unique_lock<std::mutex> lock(criticalSection);

	if (readyBlocks.empty())
	{
		return nullptr;
	}

	if (usedBlock != nullptr)
	{
		usedBlocks.push_back(usedBlock);
	}

	std::vector<char>* newBlock = readyBlocks.front();
	readyBlocks.pop_front();

	return newBlock;
}

std::vector<char>* Router::tryRotateReadyBlocks(std::vector<char>* readyBlock)
{
	std::unique_lock<std::mutex> lock(criticalSection);

	if (usedBlocks.empty())
	{
		return nullptr;
	}

	if (readyBlock != nullptr)
	{
		readyBlocks.push_back(readyBlock);
	}

	std::vector<char>* newBlock = usedBlocks.front();
	usedBlocks.pop_front();

	return newBlock;
}
