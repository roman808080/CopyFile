#include "SharedMemory.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <thread>

#include "SharedMemoryBuffer.h"

SharedMemory SharedMemory::createSharedMemory(const std::string &sharedMemoryName)
{
    // Create a shared memory object.
    shared_memory_object shm(open_or_create, sharedMemoryName.c_str(), read_write);
    SharedMemory sharedMemory(sharedMemoryName, std::move(shm));

    sharedMemory.initMemoryBuffer();
    return std::move(sharedMemory);
}

 SharedMemory SharedMemory::attachSharedMemory(const std::string &sharedMemoryName)
{
    // Attached to a shared memory.
    shared_memory_object shm(open_only, sharedMemoryName.c_str(), read_write);
    SharedMemory sharedMemory(sharedMemoryName, std::move(shm));

    sharedMemory.castMemoryBuffer();
    return std::move(sharedMemory);
}

SharedMemory::SharedMemory(SharedMemory&& other)
: sharedMemoryName(std::move(other.sharedMemoryName))
, shm(std::move(other.shm))
, region(std::move(other.region))
, data{other.data}
{
    other.data = nullptr;
}

SharedMemory& SharedMemory::operator=(SharedMemory&& other)
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

SharedMemory::~SharedMemory()
{
    if (sharedMemoryName != "")
    {
        shared_memory_object::remove(this->sharedMemoryName.c_str());
    }
}

SharedMemoryBuffer* SharedMemory::get()
{
    return data;
}

SharedMemory::SharedMemory(const std::string &sharedMemoryName, shared_memory_object &&shm)
: sharedMemoryName(sharedMemoryName)
, shm(std::move(shm)) {
    this->shm.truncate(sizeof(SharedMemoryBuffer));
    this->region = std::move(mapped_region(this->shm, read_write));
}

void SharedMemory::initMemoryBuffer()
{
    // Get the address of the mapped region
    void* addr = region.get_address();

    // Construct the shared structure in memory
    data = new (addr) SharedMemoryBuffer; 
}

void SharedMemory::castMemoryBuffer()
{
    // Get the address of the mapped region
    void* addr = region.get_address();

    // Obtain the shared structure
    data = static_cast<SharedMemoryBuffer *>(addr);
}

namespace
{
    void tryReadToSharedMemory(InputFile& inputFile, SharedMemoryBuffer* data)
    {
        int iteration = 0;
        while (!inputFile.isFinished())
        {
            while(!data->empty.try_wait())
            {
                if (data->isFailed())
                {
                    throw std::runtime_error("Failed to copy file.");
                }

                std::this_thread::yield();
            }

            iteration = iteration % SharedMemoryBuffer::NumItems;
            auto item = &data->items[iteration];
            inputFile.readBlock(item);

            data->stored.post();

            ++iteration;
        }

        data->empty.wait();
        data->items[iteration % SharedMemoryBuffer::NumItems].size = 0;
        data->stored.post();
    }
}

void readFromFileToSharedMemory(InputFile& inputFile, SharedMemoryBuffer* data)
{
    try
    {
        tryReadToSharedMemory(inputFile, data);
    }
    catch(const std::exception& e)
    {
        data->setFailed();
        throw;
    }
}

namespace
{
    void tryReadFromSharedMemory(OutputFile& outputFile, SharedMemoryBuffer* data)
    {
        // Extract the data
        int iteration = 0;
        while (true)
        {
            while(!data->stored.try_wait())
            {
                if (data->isFailed())
                {
                    throw std::runtime_error("Failed to copy file.");
                }

                std::this_thread::yield();
            }

            iteration = iteration % SharedMemoryBuffer::NumItems;
            auto item = &data->items[iteration];
            if (item->size == 0)
            {
                return;
            }

            outputFile.write(item);

            data->empty.post();
            ++iteration;
        }
    }
}

void writeFromSharedMemoryToFile(OutputFile& outputFile, SharedMemoryBuffer* data)
{
    try
    {
        tryReadFromSharedMemory(outputFile, data);
    }
    catch(const std::exception& e)
    {
        data->setFailed();
        throw;
    }
}
