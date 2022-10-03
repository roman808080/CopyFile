#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

#include "anonymous_semaphore_shared_data.h"

using namespace boost::interprocess;

class SharedMemory
{
public:

    static std::unique_ptr<SharedMemory> tryCreateSharedMemory(const std::string &sharedMemoryName);
    static std::unique_ptr<SharedMemory> attachSharedMemory(const std::string &sharedMemoryName);

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