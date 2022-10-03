#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/scope_exit.hpp>

#include "anonymous_semaphore_shared_data.h"
#include "OutputFile.h"
#include "InputFile.h"

using namespace boost::interprocess;

class SharedMemory
{
public:

    static std::unique_ptr<SharedMemory> tryCreateSharedMemory(const std::string &sharedMemoryName)
    {
        try
        {
            shared_memory_object shm(create_only, sharedMemoryName.c_str(), read_write);
            std::unique_ptr<SharedMemory> sharedMemory(std::make_unique<SharedMemory>(sharedMemoryName, std::move(shm)));

            sharedMemory->initMemoryBuffer();
            return std::move(sharedMemory);
        }
        catch(const std::exception& e)
        {
            return nullptr;
        }
    }

    static std::unique_ptr<SharedMemory> attachSharedMemory(const std::string &sharedMemoryName)
    {
        // Create a shared memory object.
        shared_memory_object shm(open_only, sharedMemoryName.c_str(), read_write);
        std::unique_ptr<SharedMemory> sharedMemory(std::make_unique<SharedMemory>(sharedMemoryName, std::move(shm)));

        sharedMemory->castMemoryBuffer();
        return std::move(sharedMemory);
    }

    SharedMemory(const std::string &sharedMemoryName, shared_memory_object &&shm)
    : sharedMemoryName(sharedMemoryName)
    , shm(std::move(shm)) {
        this->shm.truncate(sizeof(shared_memory_buffer));
        this->region = std::move(mapped_region(this->shm, read_write));
    }

    SharedMemory(const SharedMemory&) = delete;
    SharedMemory& operator=(const SharedMemory&) = delete;

    SharedMemory(SharedMemory&& other)
    : sharedMemoryName(std::move(other.sharedMemoryName))
    , shm(std::move(other.shm))
    , region(std::move(other.region))
    , data{other.data}
    {
        other.data = nullptr;
    }

    SharedMemory& operator=(SharedMemory&& other)
    {
        if (&other == this)
        {
            return *this;
        }

        this->sharedMemoryName = std::move(other.sharedMemoryName);
        this->shm = std::move(other.shm);
        this->region = std::move(other.region);

        this->data = other.data;
        other.data = nullptr;

        return *this;
    }

    ~SharedMemory()
    {
        if (sharedMemoryName != "")
        {
            shared_memory_object::remove(this->sharedMemoryName.c_str());
        }
    }

    shared_memory_buffer* get()
    {
        return data;
    }

private:
    void initMemoryBuffer()
    {
        // Get the address of the mapped region
        void* addr = region.get_address();

        // Construct the shared structure in memory
        data = new (addr) shared_memory_buffer; 
    }

    void castMemoryBuffer()
    {
        // Get the address of the mapped region
        void* addr = region.get_address();

        // Obtain the shared structure
        data = static_cast<shared_memory_buffer *>(addr);
    }

private:
    std::string sharedMemoryName;
    shared_memory_object shm;
    mapped_region region;
    shared_memory_buffer* data;
};