#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

#include "OutputFile.h"
#include "InputFile.h"

using namespace boost::interprocess;

struct shared_memory_buffer;

class SharedMemory
{
public:

    static SharedMemory createSharedMemory(const std::string &sharedMemoryName);
    static SharedMemory attachSharedMemory(const std::string &sharedMemoryName);

    SharedMemory(const std::string &sharedMemoryName, shared_memory_object &&shm);

    SharedMemory(const SharedMemory&) = delete;
    SharedMemory& operator=(const SharedMemory&) = delete;

    SharedMemory(SharedMemory&& other);
    SharedMemory& operator=(SharedMemory&& other);
    ~SharedMemory();

    shared_memory_buffer* get();

private:
    void initMemoryBuffer();
    void castMemoryBuffer();

private:
    std::string sharedMemoryName;
    shared_memory_object shm;
    mapped_region region;
    shared_memory_buffer* data;
};

void readFromFileToSharedMemory(InputFile& inputFile, shared_memory_buffer* data);
void writeFromSharedMemoryToFile(OutputFile& outputFile, shared_memory_buffer* data);
