#pragma once

#include <list>
#include <memory>
#include <vector>
#include <mutex>

class Router
{
public:
	Router();

	// Utilizes a used block and gives a new block to write.
	// This method for writing thread.
	std::vector<char>* rotateUsedBlocks(std::vector<char>* usedBlock);

	// Get a block of memory which can be used for writing.
	// This method for reading thread. 
	std::vector<char>* rotateReadyBlocks(std::vector<char>* readyBlock);

private:
	std::vector<char>* tryRotateUsedBlocks(std::vector<char>* usedBlock);
	std::vector<char>* tryRotateReadyBlocks(std::vector<char>* readyBlock);

private:
	std::vector<std::vector<char>> cache;

	std::list<std::vector<char>*> readyBlocks;
	std::list<std::vector<char>*> usedBlocks;

	std::mutex criticalSection;
};

