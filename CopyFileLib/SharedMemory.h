#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

#include "OutputFile.h"
#include "InputFile.h"

using namespace boost::interprocess;

struct SharedMemoryBuffer;

class SharedMemory
{
public:

    static SharedMemory createSharedMemory(const std::string &sharedMemoryName);
    static SharedMemory attachSharedMemory(const std::string &sharedMemoryName);

    SharedMemory(const SharedMemory&) = delete;
    SharedMemory& operator=(const SharedMemory&) = delete;

    SharedMemory(SharedMemory&& other);
    SharedMemory& operator=(SharedMemory&& other);
    ~SharedMemory();

    SharedMemoryBuffer* get();

private:
    SharedMemory(const std::string &sharedMemoryName, shared_memory_object &&shm);

    void initMemoryBuffer();
    void castMemoryBuffer();

private:
    std::string sharedMemoryName;
    shared_memory_object shm;
    mapped_region region;
    SharedMemoryBuffer* data;
};

void readFromFileToSharedMemory(InputFile& inputFile, SharedMemoryBuffer* data);
void writeFromSharedMemoryToFile(OutputFile& outputFile, SharedMemoryBuffer* data);
