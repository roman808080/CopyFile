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
		usedOutputBlocks.push_back(&cache[i]);
	}
}


std::vector<char>* Router::rotateInputBlocks(std::vector<char>* readyBlock)
{
	std::vector<char>* newBlock = nullptr;
	while ((newBlock = tryRotateInputBlocks(readyBlock)) == nullptr)
	{
		std::this_thread::yield();
	}

	return newBlock;
}

std::vector<char>* Router::rotateOutputBlocks(std::vector<char>* usedBlock)
{	
	std::vector<char>* newBlock = nullptr;
	while ((newBlock = tryRotateOutputBlocks(usedBlock)) == nullptr)
	{
		std::this_thread::yield();
	}

	return newBlock;
}

std::vector<char>* Router::tryRotateInputBlocks(std::vector<char>* readyBlock)
{
	std::unique_lock<std::mutex> lock(criticalSection);

	if (usedOutputBlocks.empty())
	{
		return nullptr;
	}

	if (readyBlock != nullptr)
	{
		readyOutputBlocks.push_back(readyBlock);
	}

	std::vector<char>* newBlock = usedOutputBlocks.front();
	usedOutputBlocks.pop_front();

	return newBlock;
}

std::vector<char>* Router::tryRotateOutputBlocks(std::vector<char>* usedBlock)
{
	std::unique_lock<std::mutex> lock(criticalSection);

	if (readyOutputBlocks.empty())
	{
		return nullptr;
	}

	if (usedBlock != nullptr)
	{
		usedOutputBlocks.push_back(usedBlock);
	}

	std::vector<char>* newBlock = readyOutputBlocks.front();
	readyOutputBlocks.pop_front();

	return newBlock;
}

