#pragma once

#include <list>
#include <memory>
#include <vector>
#include <mutex>

class Router
{
public:
	Router();

	// The method gets a block of memory which can be used for writing.
	// This method for reading thread. 
	std::vector<char>* rotateInputBlocks(std::vector<char>* readyBlock);

	// The method utilizes a used block and gives a new block to write.
	// This method for writing thread.
	std::vector<char>* rotateOutputBlocks(std::vector<char>* usedBlock);

private:
	// locks and rotates blocks
	std::vector<char>* tryRotateInputBlocks(std::vector<char>* readyBlock);
	std::vector<char>* tryRotateOutputBlocks(std::vector<char>* usedBlock);

private:
	// An initialized chunk of memory from which output and input blocks are taken.
	std::vector<std::vector<char>> cache;

	// Blocks which are ready for writing into a file.
	std::list<std::vector<char>*> readyOutputBlocks;

	// The list is used to give away used blocks to reuse for reading.
	std::list<std::vector<char>*> usedOutputBlocks;

	std::mutex criticalSection;
};

