#pragma once

#include <list>
#include <memory>
#include <vector>
#include <mutex>
#include <atomic>

struct Chunk
{
	char* startPosition;
	size_t size;
};

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
	Chunk rotateInputBlocks(Chunk readyBlock);

	// The method utilizes a used block and gives a new block to write.
	// This method for writing thread.
	Chunk rotateOutputBlocks(Chunk usedBlock);
	
	void stopRotation();
	bool isRotationStopped();

private:
	// locks and rotates blocks
	Chunk tryRotateInputBlocks(Chunk readyBlock);
	Chunk tryRotateOutputBlocks(Chunk usedBlock);

private:
	// An initialized chunk of memory from which output and input blocks are taken.
	std::vector<char> cache;

	// Blocks which are ready for writing into a file.
	std::list<Chunk> readyOutputBlocks;

	// The list is used to give away used blocks to reuse for reading.
	std::list<Chunk> usedOutputBlocks;

	std::mutex criticalSection;

	// to stop yielding if necessary
	std::atomic_bool stopped = false;
};

