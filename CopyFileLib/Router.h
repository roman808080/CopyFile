#pragma once

#include <list>
#include <memory>
#include <vector>
#include <mutex>

class Router
{
public:
	Router();
	~Router();

	// The class has an std::atomic_bool variable which cannot be moved or copied.
	// So, it is a good idea to block the same behavior for the whole class to avoid strange errors.
	Router(const Router&) = delete;
    Router& operator=(const Router&) = delete;
    Router(Router&&) = delete;
    Router& operator=(Router&&) = delete;

	// The method gets a block of memory which can be used for reading.
	// This method for reading thread. 
	std::vector<char>* rotateInputBlocks(std::vector<char>* readyBlock);

	// The method utilizes a used block and gives a new block to write.
	// This method for writing thread.
	std::vector<char>* rotateOutputBlocks(std::vector<char>* usedBlock);
	
	void stopRotation();
	bool isRotationStopped();

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

	// to stop yielding if necessary
	std::atomic_bool stopped = false;
};

